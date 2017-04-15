#ifndef __PTE__
#define __PTE__

struct PTE{
	public:
	unsigned int present:1; // if it is present in frame
	unsigned int ref:1;
	unsigned int mod:1;
	unsigned int pageout:1; // if it has been swapped out 
	unsigned int frameind:8; // if it has been swapped out 
	
	PTE():present(0), ref(0),mod(0),pageout(0), frameind(0) {}
};
struct Frame{

	public:
	unsigned int haspage:1;
	unsigned int pageind : 7; // 0-63
	Frame(): haspage(0) , pageind(0){}
};
#endif