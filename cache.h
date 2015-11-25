/*#ifndef CACHE_H
#define CACHE_H
*/
#define MAX_S 2223
#define MAX_A 101
#define INF 99999999

class cache {

public:
	int blocksize;
	int size;
	int assoc;
	int replacement_policy;
	int write_policy;
	
	int cache_blocks;
	int set;
	int index_bits;
	int block_offset_bits;
	int tag_bits;
	
	unsigned int tag[MAX_S][MAX_A];
	bool flagD[MAX_S][MAX_A];
	bool flagV[MAX_S][MAX_A];
	int counter[MAX_S][MAX_A];
	int count_set[MAX_S];
	int a,b,c,d,f,g;
	double e, aat;

	cache *nextLevel;

public:
	void setData(int a, int b, int c, int d, int f);
	int hit(unsigned int add);
	int invalid(unsigned int add);
	int LRU(unsigned int add);
	int replaceLRU(unsigned int add, unsigned int tag1);
	int replaceLFU(unsigned int index, unsigned int tag1);
	void read(unsigned int add);
	void write(unsigned int add);

};
/*
#endif //CACHE_H*/