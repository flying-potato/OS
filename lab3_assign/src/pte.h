#ifndef __PTE__
#define __PTE__

class PTE{
	public:
	unsigned int ref:1;
	unsigned int mod:1;
	unsigned int pageout:1; // if it has been swapped out 
	unsigned int framenum:8; // if it has been swapped out 
	
	PTE():ref(0),mod(0),pageout(0), framenum(0) {}
};

#endif