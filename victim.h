#define MAX_S 2223
#define MAX_A 101
#define INF 99999999

class victim {

public:
	int blocksize;
	int size;
	
	int cache_blocks;
	int block_offset_bits;
	int tag_bits;
	
	unsigned int tag[MAX_S];
	bool flagD[MAX_S];
	bool flagV[MAX_S];
	int counter[MAX_S];
	int f;
	double e, aat;

public:
	void setData(int a, int b);
	int hit(unsigned int add);
	int invalid();
	int LRU();
	int replaceLRU(unsigned int add, unsigned int tag1);
	void read(unsigned int add);
	void write(unsigned int add);

};