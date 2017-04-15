#define VP (64)

#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <sstream>
#include <cstring>
#include <string>
#include <cctype>
#include <iomanip>
#include <map>


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "PTE.h"
#include "simulation.h"
// #include "Pager.h"
using namespace std;

void changeBitmap(map<char, bool> flagbitmap, string& out_flagbitstr){
	for (int i = 0 ; i<out_flagbitstr.size(); i++){
		char bit =  out_flagbitstr[i] ;
		flagbitmap[bit] = true; 
	}
}

void initFlagbits( map<char, bool> mymap, string flagbitstr ){
	// for (std::map<char,bool>::iterator it=mymap.begin(); it!=mymap.end(); ++it){
	// 	cout<<it->first<<it->second<<endl;
	// }

}


int main( int argc, char* argv[] )
{
	
    // struct PTE* ptable[64] ;
	// bool printOut=false, printPage=false, printFrame=false, printSum=false, printEachPage=false,printEachFrame=false,printEachAging=false;

	map<char, bool> flagbitmap;
	string flagbit_str = "OPFS" ; //all bit char
	for(int i = 0; i<flagbit_str.size(); i++){
		flagbitmap[ flagbit_str[i] ]  = false; // init all bit char to false
	}

	stringstream tmp_stream;
	char algo; int framenum; string out_flagbitstr;
	char *infile_name = NULL;
    char *randfile_name = NULL;
    char* algo_ptr = NULL, *out_ptr = NULL, *framenum_ptr = NULL;
    char tmp;
    while((tmp=getopt(argc,argv,"a:o:f:"))!=-1) //read every option entry
    {
        switch(tmp){
        case 'a': //replace alg
            //algo_ptr = optarg;
			// algo_ptr = optarg;
			algo = *optarg;
			cout<<algo<<endl;
            break;
		case 'o': 
			// out_ptr = optarg;
			out_flagbitstr = optarg;
			// cout<<out_flagbitstr<<endl;	
			changeBitmap(flagbitmap , out_flagbitstr) ; 
			break;
		case 'f':
			// framenum_ptr = optarg; 
			tmp_stream << optarg;
			tmp_stream >> framenum ;
			cout<< framenum<< endl;
			break;
        }
    }
    if (optind < argc){
        infile_name = argv[optind];
        optind++;
        randfile_name = argv[optind];
    }

	vector<PTE*> ptable;
	for (int i = 0; i<VP ;i++){ //i means its pageindex, visit ptable[i]
		ptable.push_back(new PTE()) ;
	}

	vector<Frame* > ftable;
	for (int i = 0; i<framenum ;i++){ //i means its pageindex, visit ptable[i]
		ftable.push_back(new Frame()) ;
	}


	// simulation begins
	// Pager* pager = new FIFO();
	ifstream infile, randfile;
	infile.open(infile_name) ; 
	randfile.open(randfile_name);


	simulation( infile, ptable, ftable);

	infile.close();
	randfile.close();
}
