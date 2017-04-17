#ifndef __SIM__
#define __SIM__

#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <sstream>

#include <string>
#include <cctype>
#include <iomanip>
#include <map>
#include <unistd.h>

#include "PTE.h"
#include "Frame.h"
#include "Pager.h"
using namespace std;

struct stat{
	unsigned int  unmaps, maps, ins, outs, zeros ;
	unsigned long long int totalcost;
	stat(): unmaps(0), maps(0), ins(0), outs(0), zeros(0) , totalcost(0){}
} stats;

int Frame::firstFreeInd = 0;

//ordered vector of frame


bool get_next_instr(ifstream& infile, int& rw, int& pageindex, int& instrind);
Frame* get_frame( Pager* pager, int framenum,  vector<Frame* >& ftable );
Frame* allocate_frame_from_free_list(int framenum, vector<Frame* >&  ftable);
void printFrameTable(vector<Frame* >&  ftable);
void printPageTable( vector<PTE* >&  ptable );

void change_page_index_in_fto(vector<int>& ftable_ordered, int pageind,int frameind) ;
void printOrderedFrametable(vector<int> ftable_ordered);


void simulation(Pager* pager, ifstream& infile,
        vector<PTE*>& ptable, int framenum, vector<Frame* >&  ftable , vector<int>& ftable_ordered){

	int rw,  pageindex, instrind = 0;

	while(get_next_instr(infile, rw, pageindex, instrind))
	{
		PTE* newpte = ptable[pageindex] ;

		if(! newpte->present ){ // now not in frame table, need new frame to load the page

			//test whether pte is present , if yes read next instruction
			Frame *oldframe , *newframe ;
			newframe = get_frame(pager, framenum, ftable ) ;
			// cout<< newframe->frameind << newpte->pageind <<endl;

			PTE* oldpte = newframe-> pageptr ; //has a PTE in the frame
			if(oldpte != NULL){
				printf("%d:\tUNMAP\t%d\t%d\n", instrind, oldpte->pageind, newframe->frameind);
                stats.unmaps++;
				oldpte-> present = 0; //no longer present in frame
				if(oldpte->mod) {
					oldpte-> pageout = 1;
					printf("%d:\t OUT\t%d\t%d\n", instrind, oldpte->pageind, newframe->frameind);
                    stats.outs++;
				}
				//process oldpte, modified? if pageout = 1, frameind reset to be -1
			}

			//if newpte pageout
			if(newpte->pageout){
				printf("%d:\tIN\t%d\t%d\n", instrind, newpte->pageind, newframe->frameind);
                stats.ins++;
			}
			else{
				printf("%d:\tZERO\t \t%d\n", instrind, newframe->frameind);
			    stats.zeros++;
            }
			printf("%d:\tMAP   \t%d\t%d\n", instrind, newpte->pageind, newframe->frameind);
            stats.maps++;

            //change newpte flagbit
            newframe->pageptr = newpte;
            newframe->haspage = 1;

            newpte->frameind = newframe->frameind;
            newpte->present = 1;
            newpte->ref = 1;
            if(rw == 1) {
                newpte->mod = 1;
            }else{
                newpte->mod = 0;
            }

			
			
			change_page_index_in_fto(ftable_ordered, newpte->pageind, newframe->frameind );
			
			


		}
		printPageTable(ptable);
		cout<< endl;
		printOrderedFrametable(ftable_ordered); cout<< " || "; printFrameTable(ftable) ;
		//option p f

		instrind ++ ; //after read an instruction

	}


    stats.totalcost= 400*(stats.unmaps+stats.maps) +
        3000*(stats.ins+stats.outs) + 150*(stats.zeros) + instrind;
	

	printPageTable(ptable); cout<<endl;
    printOrderedFrametable(ftable_ordered);    cout<<endl;

	printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",
		instrind, stats.unmaps, stats.maps, stats.ins, stats.outs, stats.zeros, stats.totalcost);
}

Frame* get_frame( Pager* pager, int framenum,  vector<Frame* >& ftable ) {
	Frame* ret_frame = allocate_frame_from_free_list(framenum , ftable) ;
	if(ret_frame == NULL ){
		ret_frame = pager->allocate_frame(ftable) ;
	}
	return ret_frame ;
}

Frame* allocate_frame_from_free_list(int framenum, vector<Frame* >&  ftable){
	if( (ftable[framenum-1])->pageptr != NULL )
		return NULL; //no free frame
	else{
		Frame* ret = ftable[Frame::firstFreeInd] ;
		Frame::firstFreeInd ++ ;
		return ret ;
	}
}

bool get_next_instr(ifstream& infile, int& rw, int& pageindex , int& instrind){
	string line;  // store the line that not start with #
	stringstream linestream;
	while ( getline(infile, line)  ){
		if (line[0] == '#')
		{
			continue;
		}
		linestream.clear();
		linestream<< line;
		linestream>> rw >> pageindex;
		printf( "==> inst: %d %d\n",rw , pageindex );

		return true;
	}
	return false;

}

void printFrameTable(vector<Frame* >&  ftable){
    for (unsigned i = 0; i<ftable.size(); i++)
    {
        cout<< " "<< ftable[i]->frameind ;
    }
	cout<<endl;
}
void printPageTable( vector<PTE* >&  ptable ) //according to frameind order
{
    for (unsigned i = 0; i<ptable.size(); i++)
    {
        PTE* pp  = ptable[i] ;
        if(pp->present){
            char R, M, S;
            R = ((pp->ref)? 'R':'-');
            M = ((pp->mod)? 'M':'-');
            S = ((pp->pageout)? 'S':'-');
            printf( "%d:%c%c%c ", i,R,M,S  );
        }else{
            printf("%c ", '*');
        }
    }
}
void change_page_index_in_fto(vector<int>& ftable_ordered,  int pageind,int frameind) {
	if(ftable_ordered.size() <= frameind){ //frameid exceed the last index
		ftable_ordered.push_back(pageind) ;
	}
	else{
		ftable_ordered[frameind] = pageind ;
	}
}
void printOrderedFrametable(vector<int> ftable_ordered){
	for (unsigned i = 0; i<ftable_ordered.size() ;i++){
		// char pr = (ftable_ordered[i] == -1? '*':char(ftable_ordered[i]));
		// printf("%c ", pr );
		cout<< ftable_ordered[i]<<" ";
	}
	
}

#endif


