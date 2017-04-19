#ifndef __PAGER__
#define __PAGER__

#include <iostream>
#include "PTE.h"
#include "Pager.h"
#include "Frame.h"
#include <queue>
#include <string>

using namespace std;

class Pager{
	protected:
	string mode;
    map<char, bool>* flagbitmap_ptr;
	public:
	Pager( map<char, bool>* in_flagbitmap_ptr ):flagbitmap_ptr(in_flagbitmap_ptr) { }
	virtual Frame* allocate_frame(vector<Frame* >& ftable ){   }

	virtual void printFrameInfoPager(vector<Frame* >&  ftable){ //for FIFO printing frame index 
		cout<< " || ";
		for (unsigned i = 0; i<ftable.size(); i++)
		{
			if( ftable[i]->pageptr != NULL ){
                printf("%d ", ftable[i]->frameind );
			}
		}

	}

    void get_flagbitmapRef(map<char, bool>& flagbitmap){
        flagbitmap = *flagbitmap_ptr;
    }
    virtual int min_aging(vector<Frame* >& ftable ){}
	virtual ~Pager(){} //delete[]
};

class FIFO: public Pager{

	public:
	FIFO( map<char, bool>* in_flagbitmap_ptr):Pager(in_flagbitmap_ptr){ mode = "f"; }
	Frame* allocate_frame(vector<Frame* >& ftable ){
        map<char, bool>&  flagbitmap = *flagbitmap_ptr;
		Frame* zeroed_frame = *(ftable.begin()) ;
		ftable.push_back(zeroed_frame) ;
		ftable.erase(ftable.begin()) ;
		return zeroed_frame;

		//Frame(int in_frameind):
	}

};


class SeconChance: public Pager{

	public:
	SeconChance( map<char, bool>* in_flagbitmap_ptr):Pager(in_flagbitmap_ptr){mode = "s"; }
	Frame* allocate_frame(vector<Frame* >& ftable ){
        map<char, bool>&  flagbitmap = *flagbitmap_ptr;
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
    vector<int>* rand_ptr; int framenum; int random_index;
	public:
	Random( int frnum , vector<int> * in_rand_ptr , map<char, bool>* in_flagbitmap_ptr):Pager(in_flagbitmap_ptr){
        mode = "r";
        framenum = frnum;
        rand_ptr = in_rand_ptr;
        random_index = 0;
    }
	Frame* allocate_frame(vector<Frame* >& ftable ){
        map<char, bool>&  flagbitmap = *flagbitmap_ptr;
        vector<int> & rand = *rand_ptr;
        int randnum = rand[ random_index ];
        int modulo = randnum % framenum;
        random_index++;
        if(random_index >= rand.size())
            random_index = 0;
		Frame* zeroed_frame = ftable[modulo];
        return zeroed_frame ;
	}
	void printFrameInfoPager(vector<Frame* >&  ftable){} //do nothing
};


class Clock_c: public Pager{
    private:
    int framenum;
    int hand;
	public:
	Clock_c(int frnum, map<char, bool>* in_flagbitmap_ptr):Pager(in_flagbitmap_ptr){
        mode = "c";
        //rand_ptr = in_rand_ptr;
        framenum = frnum;
        hand = 0;
    }
	Frame* allocate_frame(vector<Frame* >& ftable ){
        map<char, bool>&  flagbitmap = *flagbitmap_ptr;

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
    int hand;
	public:
	Clock_X(int frnum , vector<PTE*>* in_ptable_ptr, map<char, bool>* in_flagbitmap_ptr):Pager(in_flagbitmap_ptr){
        mode = "X";
        framenum = frnum;
        ptable_ptr = in_ptable_ptr ;
        //rand_ptr = in_rand_ptr;
        hand = 0;

    }
	Frame* allocate_frame(vector<Frame* >& ftable ){
//hand is pageind of the first unref page
// hand init on 0, select min index page, replace it and hand++, or simply hand++
        map<char, bool>&  flagbitmap = *flagbitmap_ptr;
        vector<PTE*>& ptable = *ptable_ptr ;
        PTE* handon_pte = ptable[ hand ];
        Frame* ret_frame  ;
        while( (!handon_pte->present) || (handon_pte->present &&handon_pte->ref == 1) )
        {
            if (handon_pte->present &&(handon_pte->ref == 1))
            { handon_pte->ref = 0; }
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

// NRU , R- RM class R- is lower
//
class NRU : public Pager
{
    private:
    int framenum;
    vector<PTE*>* ptable_ptr;
    vector<int>* rand_ptr;
    int random_index;
    int selclass,  selidx ;
    int clock ; //10 cycle reset ref bit
	public:
	NRU (int frnum , vector<PTE*>* in_ptable_ptr, vector<int>* in_rand_ptr, map<char, bool>* in_flagbitmap_ptr):Pager(in_flagbitmap_ptr){
        selclass = 0;
        selidx= 0 ;
        clock = 0;
        mode = "N";
        framenum = frnum;
        ptable_ptr = in_ptable_ptr ;
        rand_ptr = in_rand_ptr;
        random_index = 0;
        
        // cout<<flagbitmap['a']<<"~~~aging"<<endl;
    }
	Frame* allocate_frame(vector<Frame* >& ftable ){
//hand is pageind of the first unref page
// hand init on 0, select min index page, replace it and hand++, or simply hand++
        map<char, bool>&  flagbitmap = *flagbitmap_ptr;
        vector<PTE*>& ptable = *ptable_ptr ;
        vector<int> & rand = *rand_ptr;
        Frame* ret_frame  ;

        vector< vector<PTE*> > prio(4, vector<PTE*>());
        prio.clear();
        for (int i = 0; i < ptable.size(); i++) {
            if (ptable[i]->present) {
                if (ptable[i]->ref == 0 && ptable[i]->mod == 0) {
                    prio[0].push_back(ptable[i]);
                } else if (ptable[i]->ref == 0 && ptable[i]->mod == 1) {
                    prio[1].push_back(ptable[i]);
                } else if (ptable[i]->ref == 1 && ptable[i]->mod == 0) {
                    prio[2].push_back(ptable[i]);
                } else if (ptable[i]->ref == 1 && ptable[i]->mod == 1) {
                    prio[3].push_back(ptable[i]);
                }
            }
        }
        //select PTE from prio  ->   ret_frame
        for (int i = 0; i<4; i++){
            if(!prio[i].empty()){
                int randnum = rand[ random_index ];

                selidx = randnum % (prio[i].size());
                selclass = i; //which class from 
                random_index++; 
                if(random_index >= rand.size())
                    random_index = 0;
                break;
            }
        }

        ret_frame = ftable[ (prio[selclass][selidx])->frameind ] ;


        if (++clock == 10) {
            for (int i = 0; i < ptable.size(); i++) {
                if( ptable[i]->present )
                    ptable[i]->ref = 0; //reset ref bit
            }
            clock = 0;
        }
        if (flagbitmap['a']) {
            if (clock == 0) {
                cout << " @@ reset NRU refbits while walking PTE" << endl;
            }
            cout << " @@ lowest_class=" << selclass << ": selidx=" << selidx << " from";
            for (int i = 0; i < prio[selclass].size(); i++) {
                cout << " " << prio[selclass][i]->pageind;
            }
            cout << endl;
        }
        return ret_frame;
	}
    void printFrameInfoPager(vector<Frame* >&  ftable){}
};

class Aging_a : public Pager{ //physical Frames aging
    private:
    int framenum;
    int min_ind;
    unsigned int REF_DELTA;

    public: 
    vector<unsigned int> counter ;
	Aging_a (int frnum , map<char, bool>* in_flagbitmap_ptr):Pager(in_flagbitmap_ptr){
        mode = "a";
        framenum = frnum;
        min_ind = -1;
        REF_DELTA  = 0x80000000 ;
        
        // cout<<flagbitmap['a']<<"~~~aging"<<endl;
    }
	Frame* allocate_frame(vector<Frame* >& ftable ){
        
        map<char, bool>&  flagbitmap = *flagbitmap_ptr;
        Frame* ret_frame  ;
        // first shift right and then make plus Ox80000000 operation by ref bit
        for ( int i = 0; i< ftable.size() ;i++ )
        {
            ftable[i]->aging >>= 1; //first shift then plus
            if (ftable[i]->pageptr->ref){
                ftable[i]->aging += REF_DELTA ;
                ftable[i]->pageptr->ref = 0; //clear ref bit
            } 
        }
        int ret_ind = min_aging(ftable) ;
        ret_frame = ftable[ret_ind] ;
        //ret_frame's aging should be 0
        ret_frame->aging = 0;
        return ret_frame ;
    }

    void printFrameInfoPager(vector<Frame* >& ftable){
        cout<< " || ";
        // need print physical frame aging information
        for (int i = 0; i< ftable.size(); i++){
            if (ftable[i]->pageptr !=NULL)
            {
                printf( "%d:%x ", i, ftable[i]->aging );
            }
        }
    }

    int min_aging(vector<Frame* >& ftable ){
        min_ind = -1;
        for(int i = 0 ;i<ftable.size(); i++){
            if( ftable[i]->pageptr->present ){
                if (min_ind == -1){
                    min_ind = i;
                    continue;
                } 
                if(ftable[min_ind]->aging > ftable[i]->aging){
                    min_ind = i ;
                }
            }
        }
        return min_ind;

    }
};

class Aging_Y : public Pager{
    private:
    int framenum;
    vector<PTE*>* ptable_ptr;
    unsigned int REF_DELTA;
    int min_page_ind , min_frame_ind;

    public: 
    int counter_size;
    vector<unsigned int> counter ;
	Aging_Y (int frnum ,  vector<PTE*>* in_ptable_ptr, map<char, bool>* in_flagbitmap_ptr):Pager(in_flagbitmap_ptr){
        mode = "Y";
        framenum = frnum;
        ptable_ptr = in_ptable_ptr ;
        min_page_ind = -1;
        min_frame_ind = -1;
        REF_DELTA  = 0x80000000 ;
        // cout<<flagbitmap['a']<<"~~~aging"<<endl;
    }
	Frame* allocate_frame(vector<Frame* >& ftable ){
        vector<PTE*>& ptable = *ptable_ptr ;
        map<char, bool>&  flagbitmap = *flagbitmap_ptr;
        Frame* ret_frame  ;

        const char aging_info[30] = " @@ min_pte = %d age=%x" ; //after execute instr aging info 

        // first shift right and then make plus Ox80000000 operation by ref bit
        for ( int i = 0; i< ptable.size() ;i++ )
        {
            if( ptable[i]->present ){

                int findex = ptable[i]->frameind ;
                ftable[ findex ]->aging >>= 1; //first shift then plus
                if (ptable[i]->ref){
                    ftable[findex]->aging += REF_DELTA ;
                    ptable[i]->ref = 0; //clear ref bit
                } 
            }
        }
        int ret_ind = min_aging(ftable) ;
        ret_frame = ftable[ret_ind] ;
        //ret_frame's aging should be 0
        ret_frame->aging = 0;
        return ret_frame ;
    }
    void printFrameInfoPager(vector<Frame* >&  ftable){
        cout<< " ||"; 
        vector<PTE*>& ptable = *ptable_ptr ;
        map<char, bool>&  flagbitmap = *flagbitmap_ptr;
        // need print physical frame aging information
        printPageAging(ftable) ;
    }
    void printPageAging (vector<Frame* >&  ftable){
        vector<PTE*>& ptable = *ptable_ptr ;
        for (int i = 0; i< ptable.size(); i++){
            if ( ptable[i]->present )
            {
                int findex = ptable[i]->frameind ;
                printf( " %d:%x", i, ftable[findex]->aging );
            }else{
                cout<< " *" ;
            }
        }
    }
    int min_aging(vector<Frame* >& ftable ){
        min_page_ind = -1; //first set to -1, to start with the first present page's aging
        vector<PTE*>& ptable = *ptable_ptr ;
        for(int i = 0 ;i<ptable.size(); i++){

            if(ptable[i]->present){
                int findex = ptable[i]->frameind ;
                if(min_page_ind == -1) { 
                    min_page_ind = i ;  
                    min_frame_ind = findex;
                    continue;
                }      
                if(ftable[min_frame_ind]->aging > ftable[findex]->aging){
                    min_page_ind = i;
                    min_frame_ind = findex ;
                }
            }

        }
        return min_frame_ind;

    }
};

#endif
