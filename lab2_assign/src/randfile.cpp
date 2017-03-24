#include <iostream>
#include <fstream>
#include "state.h"
using namespace std;

int main( int argc, char* argv[] ){
    // ifstream rfile;
    // ofstream outfile;

    // rfile.open("../rfile") ;
    // outfile.open("../randomresult.txt") ;
    // int numinline, randcount;
    // rfile>>randcount;      
    // int i = 0;
    // while(rfile>>numinline){ 
    //     // rand.push_back( numinline);
    //     outfile<<i<<"_"<<numinline<< ":"<<'\t'<<numinline%10+1<<'\t'<<numinline%90+1<<'\t';
    //     outfile<< numinline%4 <<'\n';
    //     i++ ;
    // }
    // rfile.close();
    // outfile.close();
    STATE st = READY ;
	cout<<bool(st == READY)<<endl;

	cout<< max(1,999)<< endl;
    TRANSITION trans = TRANS_TO_READY;
    switch(trans){
        case TRANS_TO_READY:
        cout<<"enter into ready"<<endl;
        break;
    }
    return 1;
}