#ifndef __PAGER__
#define __PAGER__

#include "PTE.h"
#include "Pager.h"
#include "Frame.h"
#include <queue>
#include <string>

using namespace std;

class Pager{
	protected:
	string mode;
	public:
	Pager(){}
	virtual Frame* allocate_frame(vector<Frame* >& ftable ){   }
	// virtual ~Pager(){} //delete[] 
};

class FIFO: public Pager{ 

	public:
	FIFO():Pager(){ }
	Frame* allocate_frame(vector<Frame* >& ftable ){
		Frame* zeroed_frame = *(ftable.begin()) ;
		ftable.push_back(zeroed_frame) ;
		ftable.erase(ftable.begin()) ;
		return zeroed_frame;

		//Frame(int in_frameind):
	}
};

#endif
