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
// #include "Pager.h"
using namespace std;

bool get_next_instr(ifstream& infile, int& rw, int& pageindex ); 

void simulation( ifstream& infile, vector<PTE*>& ptable,vector<Frame* >&  ftable ){
// void simulation(Pager* pager, ifstream& infile, vector<PTE*> ptable,vector<Frame* > ftable ){
	

	int rw,  pageindex; 
	while(get_next_instr(infile, rw, pageindex))
	{
		cout<< rw<<" "<<pageindex<< endl;
		//test whether pte is present , if yes read next instruction, if not seek a free frame to load it
		if( ptable[pageindex] ) //virtural page has PTE page table entry
		{
			// continue;
			// Frame *oldframe , *newframe ;
			// newframe = get_frame(&oldframe)
		}

	}

}

bool get_next_instr(ifstream& infile, int& rw, int& pageindex ){
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
		return true;
	}
	return false;

}



#endif


