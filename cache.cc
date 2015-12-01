#include "cache.h"
#include "string.h"
#include "math.h"

void cache::setData(int _a,int _b,int _c,int _d,int _f) {
    blocksize = _a;
    size = _b;
    assoc = _c;
    replacement_policy = _d;
    write_policy = _f;

    a = b = c = d = f = g = 0;
    e = 0;

    memset(tag, 0, sizeof(tag));
    memset(flagD, 0, sizeof(flagD));
    memset(flagV, 0, sizeof(flagV));

    cache_blocks = _b / _a;
    set = _b / ( _a * _c );
    index_bits = log(set) / log(2);
    block_offset_bits = log(blocksize) / log(2);
    tag_bits = 32 - index_bits - block_offset_bits;
    if(!replacement_policy) {//init count LRU
        for(int i = 0; i < set; i ++)
            for(int j = 0; j < assoc; j ++ )
                counter[i][j] = assoc- j - 1;
    }
    else {//init count LFU
        memset(counter, 0, sizeof(counter));
        memset(count_set, 0, sizeof(count_set));
    }
}

int cache::hit(unsigned int add) {
    unsigned int index = (add>>block_offset_bits) & (set-1);
    unsigned int tag1 = (add>>(block_offset_bits+index_bits));

    for(int i = 0; i < assoc; i ++) {
        if(tag1 == tag[index][i] && flagV[index][i])
            return i;
    }
    return -1;
}

int cache::invalid(unsigned int add) {
    unsigned int index = (add>>block_offset_bits) & (set-1);
    for(int i = 0; i < assoc; i ++) {
        if(!flagV[index][i])
    	return i;
    }
    return -1;
}

int cache::LRU(unsigned int add) {
    unsigned int index = (add>>block_offset_bits) & (set-1);
    int m = -1, pos;
    for(int i = 0; i < assoc; i ++)
        if(m < counter[index][i]) {
            m = counter[index][i];
            pos = i;
        }
    return pos;
}

int cache::replaceLRU(unsigned int add, unsigned int tag1) {
    unsigned int index = (add>>block_offset_bits) & (set-1);
    int pos = LRU(add);
    counter[index][pos] = 0;
    for(int i = 0; i < assoc; i ++) {
        if(i == pos) continue;
        counter[index][i] ++;
    }

    tag[index][pos] = tag1;
    flagV[index][pos] = 1;
    return pos;
}

int cache::replaceLFU(unsigned int add, unsigned int tag1) {
    unsigned int index = (add>>block_offset_bits) & (set-1);
    int m = INF, pos;
    for(int i = 0; i < assoc; i ++)
        if(m > counter[index][i]) {
            m = counter[index][i];
            pos = i;
        }
    tag[index][pos] = tag1;
    flagV[index][pos] = 1;
    //int tmp = count_set[index];
    count_set[index] = counter[index][pos];
    counter[index][pos] = count_set[index] + 1;
    return pos;
}

void cache::read(unsigned int add) {
    unsigned int index = (add>>block_offset_bits) & (set-1);
    unsigned int tag1 = (add>>(block_offset_bits+index_bits));
    a ++;
    if(hit(add)==-1) {//miss
        b ++;
        int i = invalid(add);
        if(i>-1) {//If have a invalid block
            flagV[index][i] = 1;
            tag[index][i] = tag1;
            if(replacement_policy) {//LFU
                counter[index][i] = count_set[index] + 1;
            }
            else {//LRU
                for(int j = 0; j < assoc; j ++) {
                    if(counter[index][j] < counter[index][i])
                        counter[index][j] ++;
                }
                counter[index][i] = 0;
            }
        }
        else {//replace_policy
            int tmpos;
            if(replacement_policy) //LFU
                tmpos = replaceLFU(index, tag1);
            else //LRU
                tmpos = replaceLRU(add, tag1);

            if(!write_policy) {
                if(flagD[index][tmpos]) {
                    f ++;
                    flagD[index][tmpos] = 0;
                }
            }
        }
    }
    else {//hit
        int hitpos = hit(add);
        if(replacement_policy) {//LFU
            counter[index][hitpos] ++;
        }
        else {//LRU
            for(int j = 0; j < assoc; j ++) {
                if(counter[index][j] < counter[index][hitpos])
                    counter[index][j] ++;
            }
            counter[index][hitpos] = 0;
        }
    }
}

void cache::write(unsigned int add) {
    unsigned int index = (add>>block_offset_bits) & (set-1);
    unsigned int tag1 = (add>>(block_offset_bits+index_bits));
    c ++;
    if(hit(add)==-1) {//miss
        d ++;
        int i = invalid(add);
        if(write_policy) {//WTNA
            //Nothing
        }
        else {//WBWA
            if(i > -1) {//If have a invalid block
                flagV[index][i] = 1;
                flagD[index][i] = 1;
                tag[index][i] = tag1;
                if(replacement_policy) {//LFU
                    counter[index][i] = count_set[index] + 1;
                }
                else {//LRU
                    for(int j = 0; j < assoc; j ++) {
                        if(counter[index][j] < counter[index][i])
                            counter[index][j] ++;
                    }
                    counter[index][i] = 0;
                }
            }
            else {
                int tmpos;
                if(replacement_policy) //LFU
                    tmpos = replaceLFU(index, tag1);
                else //LRU
                    tmpos = replaceLRU(add, tag1);
                if(flagD[index][tmpos])
                    f ++;
                flagD[index][tmpos] = 1;
            }
        }
    }
    else {//hit
        int i = hit(add);
        if(replacement_policy) {//LFU
            if(write_policy) {//WTNA
                tag[index][i] = tag1;
                counter[index][i] ++;
            }
            else {//WBWA
                flagD[index][i] = 1;
                tag[index][i] = tag1;
                counter[index][i] ++;
            }
        }
        else {//LRU
            if(write_policy) {//WTNA
                tag[index][i] = tag1;
                for(int j = 0; j < assoc; j ++) {
                    if(counter[index][j] < counter[index][i])
                        counter[index][j] ++;
                }
                counter[index][i] = 0;
            }
            else {//WBWA
                flagD[index][i] = 1;
                tag[index][i] = tag1;
                for(int j = 0; j < assoc; j ++) {
                    if(counter[index][j] < counter[index][i])
                        counter[index][j] ++;
                }
                counter[index][i] = 0;
            }
        }
    }
}
