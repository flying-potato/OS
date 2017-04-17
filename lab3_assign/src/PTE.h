#ifndef __PTE__
#define __PTE__

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct PTE{
	public:
	unsigned int present:1; // if it is present in frame
	unsigned int ref:1;
	unsigned int mod:1;
	unsigned int pageout:1; // if it has been swapped out 
	unsigned int pageind : 8; 
	unsigned int frameind:8; // if it has been swapped out 
	
	PTE(int in_pageind= 255):present(0), ref(0),mod(0),pageout(0), frameind(0), pageind(in_pageind) {}
};

#endif