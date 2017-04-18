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

	virtual void printFrameInfoPager(vector<Frame* >&  ftable){
		cout<< " || ";
		for (unsigned i = 0; i<ftable.size(); i++)
		{
			if( ftable[i]->pageptr != NULL ){
				cout<< ftable[i]->frameind <<" " ;
			}
		}

	}
	virtual ~Pager(){} //delete[]
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

	}
};

class Random: public Pager{
    private:
    vector<int>* rand_ptr; int framenum;
	public:
	Random(vector<int> * in_rand_ptr, int frnum):Pager(){
        mode = "r";
        rand_ptr = in_rand_ptr;
        framenum = frnum;

    }
	Frame* allocate_frame(vector<Frame* >& ftable ){
        vector<int> & rand = *rand_ptr;
        int randnum = *(rand.begin());
        int modulo = randnum % framenum;
        rand.erase(rand.begin());
		Frame* zeroed_frame = ftable[modulo];
        return zeroed_frame ;
	}
	void printFrameInfoPager(vector<Frame* >&  ftable){} //do nothing
};


class Clock_c: public Pager{
    private:
    int framenum;
	public:
    static int hand;
	Clock_c(int frnum):Pager(){
        mode = "c";
        //rand_ptr = in_rand_ptr;
        framenum = frnum;

    }
	Frame* allocate_frame(vector<Frame* >& ftable ){

		Frame* zeroed_frame = ftable[ hand ];
        while (zeroed_frame->pageptr!=NULL && zeroed_frame->pageptr->ref==1)
        {
            zeroed_frame->pageptr->ref = 0;//clear ref bit, shift hand to see next one
            hand++;
            if(hand==framenum){ hand = 0; }
            zeroed_frame = ftable[hand] ;
        }
        hand++ ;// if ref == 0, shift the hand
        if(hand==framenum){ hand = 0; }
        return zeroed_frame ;
	}

	void printFrameInfoPager(vector<Frame* >&  ftable){

		cout<< " || ";
        cout<< "hand = "<<hand ;
    } //do nothing
};

class Clock_X: public Pager{
    private:
    int framenum;
    vector<PTE*>* ptable_ptr;
	public:
    static int hand;
	Clock_X(int frnum , vector<PTE*>* in_ptable_ptr):Pager(){
        mode = "X";
        framenum = frnum;
        ptable_ptr = in_ptable_ptr ;
        //rand_ptr = in_rand_ptr;

    }
	Frame* allocate_frame(vector<Frame* >& ftable ){
//hand is pageind of the first unref page
// hand init on 0, select min index page, replace it and hand++, or simply hand++

        bool found = false;
        vector<PTE*>& ptable = *ptable_ptr ;
        PTE* handon_pte = ptable[ hand ];
        Frame* ret_frame  ;
        while( (!handon_pte->present) || (handon_pte->present &&handon_pte->ref == 1) )
        {
            if (handon_pte->present &&(handon_pte->ref == 1))
            { handon_pte->ref = 0; }
            /*if ( handon_pte->present ){
                if( handon_pte->ref == 1){
                    handon_pte->ref = 0;
                }else{
                    ret_frame = ftable[handon_pte->frameind] ;
                    found = true; //found the frame
                }
            }*/
            //not present, directly hand++, test exceed
            hand++;
            if(hand>=ptable.size()){hand = 0;}
            handon_pte = ptable[hand] ;

        }
        ret_frame = ftable[handon_pte->frameind] ;

        hand++;
        if(hand>=ptable.size()){hand = 0;}
        return ret_frame;
	}

	void printFrameInfoPager(vector<Frame* >&  ftable){

		cout<< " || ";
        cout<< "hand = "<<hand ;
    } //do nothing
};




#endif
