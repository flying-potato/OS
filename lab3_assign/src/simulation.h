#ifndef __SIM__
#define __SIM__
#define VP (64)

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

#include "pte.h"

using namespace std;

PTE*[] pteArray = ;


void simulation(Pager* pager, ifstream fd_instr){
	
	int rw, int pagenum; 
	while(get_next_instr(fd_instr, rw, pagenum))
	{
		if(!ptePresent(pagenum)) //virtural page has PTE page table entry
		{
			Frame *oldframe , *newframe ;
			newframe = get_frame(&oldframe)
		}

	}
}

bool get_next_instr(ifstream& fd_instr, int& rw, int& pagenum ){
	if( fd_instr>> rw >> pagenum ){
		return true;
	}else{
		return false;
	}
}


bool ptePresent(int& pagenum){
	if(pagenum){
		return true;

	}
}
#endif


