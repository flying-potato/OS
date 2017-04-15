#ifndef __PAGER__
#define __PAGER__

#include "PTE.h"
#include "Pager.h"

class Pager{
	public:
	Pager(){

	}

	virtual Frame* allocate_frame(Frame **frame_old );
};

class FIFO: public Pager{
	public:
	FIFO():Pager(){	}
	Frame* allocate_frame(Frame ** frame_old){return new Frame();}
};

#endif