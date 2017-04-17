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

int Clock_c::hand = 0;

//ordered vector of frame


bool get_next_instr(ifstream& infile, int& rw, int& pageindex, int& instrind);
Frame* get_frame( Pager* pager, int framenum,  vector<Frame* >& ftable );
Frame* allocate_frame_from_free_list(int framenum, vector<Frame* >&  ftable);

void printPageTable( vector<PTE* >&  ptable );

void change_page_index_in_fto(vector<int>& ftable_ordered, int pageind,int frameind) ;
void printOrderedPageInd(vector<int>& ftable_ordered,int framenum);


void simulation(Pager* pager, ifstream& infile,
        vector<PTE*>& ptable, int framenum, vector<Frame* >&  ftable , vector<int>& ftable_ordered){

	int rw,  pageindex, instrind = 0;
	const char outform[20] = "%d: %-5s%4d%4d\n";
	while(get_next_instr(infile, rw, pageindex, instrind))
	{
		PTE* newpte = ptable[pageindex] ;

		newpte->ref = 1; //has to be 1

		if(! newpte->present ){ // now not in frame table, need new frame to load the page

			//test whether pte is present , if yes read next instruction
			Frame *oldframe , *newframe ;
			newframe = get_frame(pager, framenum, ftable ) ;
			// cout<< newframe->frameind << newpte->pageind <<endl;

			PTE* oldpte = newframe-> pageptr ; //has a PTE in the frame
			if(oldpte != NULL){
				printf(outform , instrind,"UNMAP", oldpte->pageind, newframe->frameind);
                stats.unmaps++;
				oldpte-> present = 0; //no longer present in frame
				oldpte-> ref = 0;
				if(oldpte->mod) {
					oldpte-> pageout = 1;
					printf(outform , instrind,"OUT",oldpte->pageind, newframe->frameind);
                    stats.outs++;
				}
				//process oldpte, modified? if pageout = 1, frameind reset to be -1
			}

			//if newpte pageout
			if(newpte->pageout){
				printf(outform , instrind,"IN", newpte->pageind, newframe->frameind);
                //after page in the has-been-modified page, mod = 0
                newpte->mod  = 0;
                //newpte->pageout = 0;//still keep pageout
                stats.ins++;
			}
			else{
				printf("%d: %-9s%4d\n",  instrind,"ZERO", newframe->frameind);
			    stats.zeros++;
            }
			printf(outform , instrind,"MAP", newpte->pageind, newframe->frameind);
            stats.maps++;

            //change newpte flagbit
            newframe->pageptr = newpte;
            newframe->haspage = 1;

            newpte->frameind = newframe->frameind;
            newpte->present = 1;




			change_page_index_in_fto(ftable_ordered, newpte->pageind, newframe->frameind );


		}
        //after load newpte -> frame
        if(rw == 1) {
            newpte->mod = 1;
        }else{
            // newpte->mod = 0; //not change the mod
        }
		printPageTable(ptable);
		cout<< endl;
		printOrderedPageInd(ftable_ordered, framenum);  pager->printFrameInfoPager(ftable) ; cout<<endl;

		//option p f

		instrind ++ ; //after read an instruction

	}


    stats.totalcost= 400*(stats.unmaps+stats.maps) +
        3000*(stats.ins+stats.outs) + 150*(stats.zeros) + instrind;


	printPageTable(ptable); cout<<endl;
    printOrderedPageInd(ftable_ordered, framenum);    cout<<endl;

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


void printPageTable( vector<PTE* >&  ptable ) //according to frameind order
{
    for (unsigned i = 0; i<ptable.size(); i++)
    {
        PTE* pp  = ptable[i] ;
        if(!pp->present){
            if(pp->pageout){
                //not present in frame, but swapped out
                printf("%c ", '#');
            }
            else{
                printf("%c ", '*');
            }
        }
        else{
            char R, M, S;
            R = ((pp->ref)? 'R':'-');
            M = ((pp->mod)? 'M':'-');
            S = ((pp->pageout)? 'S':'-');
            printf( "%d:%c%c%c ", i,R,M,S  );
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
void printOrderedPageInd(vector<int>& ftable_ordered, int framenum){
	unsigned i = 0;
	for ( i = 0; i<ftable_ordered.size() ;i++){
		// char pr = (ftable_ordered[i] == -1? '*':char(ftable_ordered[i]));
		// printf("%c ", pr );
		cout<< ftable_ordered[i]<<" ";
	}
	for( ; i<framenum ; i++ )
	{
		cout<< "*" << " ";
	}

}

#endif


