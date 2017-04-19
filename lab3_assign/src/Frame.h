#ifndef __FRAME__
#define __FRAME__


#include "PTE.h"

struct Frame{

	public:
    //unsigned int frameind: 16;
	static int firstFreeInd ;
    int frameind ;
	unsigned int haspage:1;
	unsigned int aging ;
	PTE* pageptr ; //get all info PTE
	Frame(int in_frameind): frameind(in_frameind), haspage(0) ,pageptr(NULL) ,aging(0){}
};

#endif
