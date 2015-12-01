#include "stdio.h"
#include "string.h"
#include "cache.h"
#include "victim.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
using namespace std;

char ch, str[30][40];
char file[20];
int nos;

unsigned int add;

cache L1;
cache L2;
victim vc;

struct no {
	unsigned int tag;
	int lru;
	int flagD;
}node[MAX_S];
bool cmp(no a, no b) {
	return a.lru < b.lru;
}

void init(int argc, char *argv[]);
void printHead();
void printTail();

int main(int argc, char *argv[]) {
    if( argc < 8 ) {
        printf( "You should enter enough(8) parameters\n" );
        return 0;
    }
    init(argc, argv);
    freopen(file,"r",stdin);
    printHead();

    //int cas = 1;
    while( scanf( "%c %x", &ch, &add )!=EOF ) {
        getchar();

        //printf("%d:\n", cas++);
        if(ch == 'r') {//read
            if(vc.size && !L2.size) {//have victim cache
                if(L1.hit(add) == -1) {//L1 miss
                    int pos = L1.LRU(add);
                    unsigned int index = (add>>L1.block_offset_bits) & (L1.set-1);
                    unsigned int tag = L1.tag[index][pos];
                    unsigned int tag1 = (tag<<L1.index_bits) + index;
                    if(vc.hit(add) == -1) {//VC miss
                        if(L1.invalid(add) == -1) {//L1 not having invalid blocks
                            int pos1 = vc.LRU();
                            if(vc.flagD[pos1])
                            	vc.f ++;
                            if(L1.flagD[index][pos])
                                L1.f --;
                            vc.flagD[pos1] = L1.flagD[index][pos];
                            vc.tag[pos1] = tag1;
                            vc.counter[pos1] = 0;
						    for(int i = 0; i < vc.cache_blocks; i ++) {
						        if(i == pos1) continue;
						        vc.counter[i] ++;
						    }
                        }
                        L1.read(add);
                    }
                    else {//VC hit
           				L1.a ++;
                        L1.counter[index][pos] = 0;
                        for(int i = 0; i < L1.assoc; i ++) {
                            if(i == pos) continue;
                            L1.counter[index][i] ++;
                        }
                        int pos1 = vc.hit(add);
                        int tag2 = vc.tag[pos1];
                        for(int j = 0; j < vc.cache_blocks; j ++) {
				            if(vc.counter[j] < vc.counter[pos1])
				                vc.counter[j] ++;
				        }
				        vc.counter[pos1] = 0;
                        //swap
                        L1.tag[index][pos] = (tag2>>L1.index_bits);
                        vc.tag[pos1] = tag1;
                        int tmp = L1.flagD[index][pos];
                        L1.flagD[index][pos] = vc.flagD[pos1];
                        vc.flagD[pos1] = tmp;
                        nos ++;
                    }
                }
                else //L1 hit
                    L1.read(add);
            }
            else if(!vc.size && L2.size){
                if(L1.hit(add) == -1) {//L1 miss
                    if(L1.invalid(add) == -1) {//L1 not having invalid blocks
						unsigned int index1 = (add>>L1.block_offset_bits) & (L1.set-1);
						int pos1 = L1.LRU(add);
						unsigned int tag1 = L1.tag[index1][pos1];
                        int flag = L1.flagD[index1][pos1];

                        if(flag) {
                            unsigned int add1 = ((tag1<<L1.index_bits)+index1)<<L1.block_offset_bits;
                            L2.write(add1);
                        }
                        L1.read(add);
                        L2.read(add);
                    }
                    else {
                        L1.read(add);
                        L2.read(add);
                    }
                }
                else//L1.hit
                    L1.read(add);
            }
            else {
                if(L1.hit(add) == -1) {//L1 miss
                    if(L1.invalid(add) == -1) {//L1 not having invalid blocks
                        unsigned int index1 = (add>>L1.block_offset_bits) & (L1.set-1);
                        int pos1 = L1.LRU(add);
                        unsigned int tag1 = L1.tag[index1][pos1];
                        unsigned int tag2 = (tag1<<L1.index_bits) + index1;

                        if(vc.hit(add) == -1) {//VC miss
                            if(L1.invalid(add) == -1) {//L1 not having invalid blocks
                                int pos2 = vc.LRU();
                                if(vc.flagD[pos2]) {
                                    vc.f ++;
                                    unsigned int add1 = vc.tag[pos2]<<L1.block_offset_bits;
                                    L2.write(add1);
                                }
                                if(L1.flagD[index1][pos1])
                                    L1.f --;
                                vc.flagD[pos2] = L1.flagD[index1][pos1];
                                vc.tag[pos2] = tag2;
                                vc.counter[pos2] = 0;
                                for(int i = 0; i < vc.cache_blocks; i ++) {
                                    if(i == pos2) continue;
                                    vc.counter[i] ++;
                                }
                            }
                            L1.read(add);
                            L2.read(add);
                        }
                        else {//VC hit
                            L1.a ++;
                            L1.counter[index1][pos1] = 0;
                            for(int i = 0; i < L1.assoc; i ++) {
                                if(i == pos1) continue;
                                L1.counter[index1][i] ++;
                            }
                            int pos2 = vc.hit(add);
                            int tag3 = vc.tag[pos2];
                            for(int j = 0; j < vc.cache_blocks; j ++) {
                                if(vc.counter[j] < vc.counter[pos2])
                                    vc.counter[j] ++;
                            }
                            vc.counter[pos2] = 0;
                            //swap
                            L1.tag[index1][pos1] = (tag3>>L1.index_bits);
                            vc.tag[pos2] = tag2;
                            int tmp = L1.flagD[index1][pos1];
                            L1.flagD[index1][pos1] = vc.flagD[pos2];
                            vc.flagD[pos2] = tmp;
                            nos ++;
                        }
                    }
                    else {
                        L1.read(add);
                        L2.read(add);
                    }
                }
                else//L1.hit
                    L1.read(add);
            }
        }
        else {//write
            if(vc.size && !L2.size) {
                if(L1.hit(add) == -1) {//L1 miss
                    int pos = L1.LRU(add);
                    unsigned int index = (add>>L1.block_offset_bits) & (L1.set-1);
                    unsigned int tag = L1.tag[index][pos];
                    unsigned int tag1 = (tag<<L1.index_bits) + index;
                    if(vc.hit(add) == -1) {//VC miss
                        if(L1.invalid(add) == -1) {//L1 not have invalid blocks
                            int pos1 = vc.LRU();
                            if(vc.flagD[pos1])
                            	vc.f ++;
                            if(L1.flagD[index][pos])
                                L1.f --;
                            vc.flagD[pos1] = L1.flagD[index][pos];
                            vc.tag[pos1] = tag1;
                            vc.counter[pos1] = 0;
						    for(int i = 0; i < vc.cache_blocks; i ++) {
						        if(i == pos1) continue;
						        vc.counter[i] ++;
						    }
                        }
                    	L1.write(add);
                    }
                    else {//VC hit
                    	L1.c ++;
                        L1.counter[index][pos] = 0;
                        for(int i = 0; i < L1.assoc; i ++) {
                            if(i == pos) continue;
                            L1.counter[index][i] ++;
                        }
                        int pos1 = vc.hit(add);
                        int tag2 = vc.tag[pos1];
                        for(int j = 0; j < vc.cache_blocks; j ++) {
				            if(vc.counter[j] < vc.counter[pos1])
				                vc.counter[j] ++;
				        }
				        vc.counter[pos1] = 0;
                        //swap
                        L1.tag[index][pos] = (tag2>>L1.index_bits);
                        vc.tag[pos1] = tag1;
                        vc.flagD[pos1] = L1.flagD[index][pos];
                        L1.flagD[index][pos] = 1;
                        nos ++;
                    }
                }
                else //L1 hit
                    L1.write(add);
            }
			else if(!vc.size && L2.size){
                if(L1.hit(add) == -1) {//L1 miss
                    if(L1.invalid(add) == -1) {//L1 not having invalid blocks
						unsigned int index1 = (add>>L1.block_offset_bits) & (L1.set-1);
						int pos1 = L1.LRU(add);
						unsigned int tag1 = L1.tag[index1][pos1];
                        int flag = L1.flagD[index1][pos1];

                        if(flag) {
                            unsigned int add1 = ((tag1<<L1.index_bits)+index1)<<L1.block_offset_bits;
                            L2.write(add1);
                        }
                        L1.write(add);
                        L2.read(add);
                    }
                    else {
                        L1.write(add);
                        L2.read(add);
                    }
                }
                else //L1 hit
                    L1.write(add);
            }
            else {
                if(L1.hit(add) == -1) {//L1 miss
                    if(L1.invalid(add) == -1) {//L1 not having invalid blocks
                        unsigned int index1 = (add>>L1.block_offset_bits) & (L1.set-1);
                        int pos1 = L1.LRU(add);
                        unsigned int tag1 = L1.tag[index1][pos1];
                        unsigned int tag2 = (tag1<<L1.index_bits) + index1;

                        if(vc.hit(add) == -1) {//VC miss
                            if(L1.invalid(add) == -1) {//L1 not having invalid blocks
                                int pos2 = vc.LRU();
                                if(vc.flagD[pos2]) {
                                    vc.f ++;
                                    unsigned int add1 = vc.tag[pos2]<<L1.block_offset_bits;
                                    L2.write(add1);
                                }
                                if(L1.flagD[index1][pos1])
                                    L1.f --;
                                vc.flagD[pos2] = L1.flagD[index1][pos1];
                                vc.tag[pos2] = tag2;
                                vc.counter[pos2] = 0;
                                for(int i = 0; i < vc.cache_blocks; i ++) {
                                    if(i == pos2) continue;
                                    vc.counter[i] ++;
                                }
                            }
                            L1.write(add);
                            L2.read(add);
                        }
                        else {//VC hit
                            L1.c ++;
                            L1.counter[index1][pos1] = 0;
                            for(int i = 0; i < L1.assoc; i ++) {
                                if(i == pos1) continue;
                                L1.counter[index1][i] ++;
                            }
                            int pos2 = vc.hit(add);
                            int tag3 = vc.tag[pos2];
                            for(int j = 0; j < vc.cache_blocks; j ++) {
                                if(vc.counter[j] < vc.counter[pos2])
                                    vc.counter[j] ++;
                            }
                            vc.counter[pos2] = 0;
                            //swap
                            L1.tag[index1][pos1] = (tag3>>L1.index_bits);
                            vc.tag[pos2] = tag2;
                            vc.flagD[pos2] = L1.flagD[index1][pos1];
                            L1.flagD[index1][pos1] = 1;
                            nos ++;
                        }
                    }
                    else {
                        L1.write(add);
                        L2.read(add);
                    }
                }
                else//L1.hit
                    L1.write(add);
            }
        }
    }
    printTail();
    return 0;
}

void init(int argc, char *argv[]) {
    nos = 0;
    int sum[7];
    memset( sum, 0, sizeof(sum) );
    for(int i = 1; i < argc-1; i ++) {
        int x = 1;
        for(int j = strlen(argv[i])-1; j >= 0; j --) {
            sum[i] += x * (argv[i][j] - '0');
            x *= 10;
        }
    }
    int bs, L1_si, L1_as, vc_si, L2_si, L2_as;
    bs = sum[1], L1_si = sum[2], L1_as = sum[3];
    vc_si = sum[4], L2_si = sum[5], L2_as = sum[6];

    strcpy(file, argv[7]);
    int fileLen = strlen(file), flag = 1;
    for(int i = 0; i < fileLen; i ++) {
        if(file[i] == '.') {
            flag = 0;
            break;
        }
    }
    if(flag) {
        file[fileLen++] = '.';
        file[fileLen++] = 't';
        file[fileLen++] = 'x';
        file[fileLen++] = 't';
    }

    strcpy(str[0],"BLOCKSIZE:");
    strcpy(str[1],"L1_SIZE:");
    strcpy(str[2],"L1_ASSOC:");
    strcpy(str[3],"Victim_Cache_SIZE:");
    strcpy(str[4],"L2_SIZE:");
    strcpy(str[5],"L2_ASSOC:");
    strcpy(str[6],"trace_file:");
    strcpy(str[7],"a. number of L1 reads:");
    strcpy(str[8],"b. number of L1 read misses:");
    strcpy(str[9],"c. number of L1 writes:");
    strcpy(str[10],"d. number of L1 write misses:");
    strcpy(str[11],"e. L1 miss rate:");
    strcpy(str[12],"f. number of swaps:");
    strcpy(str[13],"g. number of victim cache writeback:");
    strcpy(str[14],"h. number of L2 reads:");
    strcpy(str[15],"i. number of L2 read misses:");
    strcpy(str[16],"j. number of L2 writes:");
    strcpy(str[17],"k. number of L2 write misses:");
    strcpy(str[18],"l. L2 miss rate:");
    strcpy(str[19],"m. number of L2 writebacks:");
    strcpy(str[20],"n. total memory traffic:");
    strcpy(str[21],"1. average access time:");

    L1.setData(bs,L1_si,L1_as,0,0);
    if(L2_si)
    	L2.setData(bs,L2_si,L2_as,0,0);
    vc.setData(bs,vc_si);
}

void printHead() {
    printf("===== Simulator configuration =====\n" );
    printf("%-22s%13d\n", str[0], L1.blocksize);
    printf("%-22s%13d\n", str[1], L1.size);
    printf("%-22s%13d\n", str[2], L1.assoc);
    printf("%-22s%13d\n", str[3], vc.size);
    printf("%-22s%13d\n", str[4], L2.size);
    printf("%-22s%13d\n", str[5], L2.assoc);
    printf("%-22s%13s\n", str[6], file);
    printf("===================================\n");
    printf("===== L1 contents =====\n");
}

void printTail() {
    for(int i = 0; i < L1.set; i ++) {
        printf("set %d:", i);
        for(int j = 0; j < L1.assoc; j ++) {
        	node[j].tag = L1.tag[i][j];
        	node[j].lru = L1.counter[i][j];
        	node[j].flagD = L1.flagD[i][j];
        }
        sort(node, node + L1.assoc, cmp);
        for(int j = 0; j < L1.assoc; j ++) {
            printf("%8x ", node[j].tag);
            if(node[j].flagD)
                printf("D");
            else
                printf(" ");
        }
        puts("");
    }
    if(vc.size) {
    	printf("===== Victim Cache contents =====\n");
	    printf("set 0:");
	    for(int j = 0; j < vc.cache_blocks; j ++) {
        	node[j].tag = vc.tag[j];
        	node[j].lru = vc.counter[j];
        	node[j].flagD = vc.flagD[j];
        }
        sort(node, node + vc.cache_blocks, cmp);
		for(int j = 0; j < vc.cache_blocks; j ++) {
		    printf("%8x ", node[j].tag);
		    if(node[j].flagD)
		        printf("D");
		    else
		        printf(" ");
		}
		puts("");
	}
    if(L2.size) {
        printf("===== L2 contents =====\n");
        for(int i = 0; i < L2.set; i ++) {
            printf("set %d:", i);
            for(int j = 0; j < L2.assoc; j ++) {
                node[j].tag = L2.tag[i][j];
                node[j].lru = L2.counter[i][j];
                node[j].flagD = L2.flagD[i][j];
            }
            sort(node, node + L2.assoc, cmp);
            for(int j = 0; j < L2.assoc; j ++) {
                printf("%8x ", node[j].tag);
                if(node[j].flagD)
                    printf("D");
                else
                    printf(" ");
            }
            puts("");
        }
    }

    L1.e = (L1.b*1.0 + L1.d*1.0) / (L1.a*1.0 + L1.c*1.0);
    L2.e = L2.b*1.0 / L2.a*1.0;
    if(L2.size) //having L2
        L2.g = L2.b + L2.d + L2.f;
    else //not having L2
        L2.g = L1.b + L1.d + L1.f + vc.f;

    if(L2.size) {
    	double ht2 = 2.5 + 2.5*(L2.size/512)/1024 + 0.025*(L2.blocksize/16) + 0.025*L2.assoc;
    	double mp2 = 20 + 0.5*(L2.blocksize/16);
    	L2.aat = ht2 + L2.e * mp2;
    	double ht1 = 0.25 + 2.5*(L1.size/512)/1024 + 0.025*(L1.blocksize/16) + 0.025*L1.assoc;
    	L1.aat = ht1 + L1.e * L2.aat;
	}
	else {
		double ht = 0.25 + 2.5*(L1.size/512)/1024 + 0.025*(L1.blocksize/16) + 0.025*L1.assoc;
	    double mp = 20 + 0.5*(L1.blocksize/16);
	    L1.aat = ht + L1.e * mp;
	}

    printf("====== Simulation results (raw) ======\n" );
    printf("%-36s%-6d\n", str[7], L1.a);
    printf("%-36s%-6d\n", str[8], L1.b);
    printf("%-36s%-6d\n", str[9], L1.c);
    printf("%-36s%-6d\n", str[10], L1.d);
    printf("%-36s%-.4lf\n", str[11], L1.e);
    printf("%-36s%-6d\n", str[12], nos);
    printf("%-36s%-6d\n", str[13], vc.f);
    printf("%-36s%-6d\n", str[14], L2.a);
    printf("%-36s%-6d\n", str[15], L2.b);
    printf("%-36s%-6d\n", str[16], L2.c);
    printf("%-36s%-6d\n", str[17], L2.d);
    if(L2.e != 0)
    	printf("%-36s%-.4lf\n", str[18], L2.e);
    else
    	printf("%-36s0\n", str[18]);
    printf("%-36s%-6d\n", str[19], L2.f);
    printf("%-36s%-6d\n", str[20], L2.g);
    printf("==== Simulation results (performance) ====\n");
    printf("%-36s%-.4lf ns\n", str[21], L1.aat );
}
