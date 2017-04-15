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
#include <time.h>

using namespace std;

int main( int argc, char* argv[] ){
    stringstream optstream;
    // char* sched_option = NULL;
    char sched_options[10]; //declare some space, use a pointer char* to point it 
    char* sched_option = sched_options ;
    char* infile_name = NULL;

    char* randfile_name = NULL;


    char tmp;
    while((tmp=getopt(argc,argv,"vs:"))!=-1) //read every option entry
    {
        switch(tmp){
        case 'a': //replace alg 
            
            break;
        case 's':
            sched_option = optarg;
            break; 
        }
    }
    if (optind < argc){
        infile_name = argv[optind];
        optind++;
        randfile_name = argv[optind];
    }   

    // string sched_str = string(sched_option) ;
    optstream<<sched_option; //can use char* as input 
    int q;
    char opt;
    optstream>>opt>>q;
    switch(opt){ //initial diff Pager
        case 'P':
            sched = new PrioSched("PRIO", q); 
            break;
        case 'F':
