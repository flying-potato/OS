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
	FIFO():Pager(){ mode = "f"; }
	Frame* allocate_frame(vector<Frame* >& ftable ){
		Frame* zeroed_frame = *(ftable.begin()) ;
		ftable.push_back(zeroed_frame) ;
		ftable.erase(ftable.begin()) ;
		return zeroed_frame;

		//Frame(int in_frameind):
	}
};


class SeconChance: public Pager{

	public:
	SeconChance():Pager(){mode = "s"; }
	Frame* allocate_frame(vector<Frame* >& ftable ){
		Frame* zeroed_frame = *(ftable.begin()) ;
        while( zeroed_frame->pageptr->ref )
        {
            zeroed_frame->pageptr->ref = 0; //clear and push it back
            ftable.push_back(zeroed_frame) ;

            ftable.erase(ftable.begin()) ;
            zeroed_frame = *(ftable.begin()) ;
        }
        ftable.push_back(zeroed_frame) ;
        ftable.erase(ftable.begin()) ;
		return zeroed_frame;

        //when first zeroed_frame ref is 1,
		//Frame(int in_frameind):
	}
};

#endif
