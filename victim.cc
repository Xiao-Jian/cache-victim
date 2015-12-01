#include "victim.h"
#include "string.h"
#include "math.h"

void victim::setData(int _a,int _b) {
    blocksize = _a;
    size = _b;
	
    f = 0;
    e = 0;
	
    memset(tag, 0, sizeof(tag));
    memset(flagD, 0, sizeof(flagD));
    /*memset(flagV, 0, sizeof(flagV));*/
	
    cache_blocks = _b / _a;
    block_offset_bits = log(blocksize) / log(2);
    tag_bits = 32 - block_offset_bits;
    for(int j = 0; j < cache_blocks; j ++ ) {
        counter[j] = j;
        flagV[j] = 1;
        flagD[j] = 0;
    }
    
}

int victim::hit(unsigned int add) {
    unsigned int tag1 = (add>>block_offset_bits);
    for(int i = 0; i < cache_blocks; i ++) {
        if(tag1 == tag[i])
            return i;
    }
    return -1;
}

int victim::invalid() {
    for(int i = 0; i < cache_blocks; i ++) {
        if(!flagV[i])
    	return i;
    }
    return -1;
}

int victim::LRU() {
    int m = -1, pos;
    for(int i = 0; i < cache_blocks; i ++)
        if(m < counter[i]) {
            m = counter[i];
            pos = i;
        }
    return pos;
}

int victim::replaceLRU(unsigned int add, unsigned int tag1) {
    int pos = LRU();
    counter[pos] = 0;
    for(int i = 0; i < cache_blocks; i ++) {
        if(i == pos) continue;
        counter[i] ++;
    }

    tag[pos] = tag1;
    flagV[pos] = 1;
    return pos;
}

void victim::read(unsigned int add) {
    unsigned int tag1 = (add>>block_offset_bits);
    if(hit(add)==-1) {//miss
        int i = invalid();
        if(i>-1) {//If have a invalid block
            flagV[i] = 1;
            tag[i] = tag1;
            for(int j = 0; j < cache_blocks; j ++) {
                if(counter[j] < counter[i])
                    counter[j] ++;
            }
            counter[i] = 0;
        }
        else {//replace_policy
            int tmpos = replaceLRU(add, tag1);
            if(flagD[tmpos]) {
                f ++;
                flagD[tmpos] = 0;
            }
        }
    }
    else {//hit
        int hitpos = hit(add);
        for(int j = 0; j < cache_blocks; j ++) {
            if(counter[j] < counter[hitpos])
                counter[j] ++;
        }
        counter[hitpos] = 0;
    }
}

void victim::write(unsigned int add) {
    unsigned int tag1 = (add>>block_offset_bits);
    if(hit(add)==-1) {//miss
        int i = invalid();
        if(i > -1) {//If have a invalid block
            flagV[i] = 1;
            flagD[i] = 1;
            tag[i] = tag1;
            for(int j = 0; j < cache_blocks; j ++) {
                if(counter[j] < counter[i])
                    counter[j] ++;
            }
            counter[i] = 0;
        }
        else {
            int tmpos = replaceLRU(add, tag1);
            if(flagD[tmpos])
                f ++;
            flagD[tmpos] = 1;
        }
    }
    else {//hit
        int i = hit(add);
        flagD[i] = 1;
        tag[i] = tag1;
        for(int j = 0; j < cache_blocks; j ++) {
            if(counter[j] < counter[i])
                counter[j] ++;
        }
        counter[i] = 0;
    }
}