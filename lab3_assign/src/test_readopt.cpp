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

using namespace std;


int main( int argc, char* argv[] )
{
	vector<PTE*> ptable;
    // struct PTE* ptable[64] ;
	bool printOut=false, printPage=false, printFrame=false, printSum=false, printEachPage=false,printEachFrame=false,printEachAging=false;

	stringstream tmp_stream;
	char algo; int framenum; string outflags;
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
			outflags = optarg;
			cout<<outflags<<endl;	
			if (outflags.find('O') != -1)  {
				printOut = true;
				// cout<<printOut<<endl;
			}

			if (outflags.find('P') != -1)  {
				printPage = true;
				cout<<printPage<<endl;
			}

			if (outflags.find('F') != -1)  {
				printFrame = true;
				// cout<<printFrame<<endl;
			}

			if (outflags.find('S') != -1)  {
				printSum = true;
				// cout<<printSum<<endl;
			}
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
}
