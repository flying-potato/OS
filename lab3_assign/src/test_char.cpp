
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

#include <map>
#include "PTE.h"
using namespace std;

void traverseMap(map<char, bool> mymap){

	for (std::map<char,bool>::iterator it=mymap.begin(); it!=mymap.end(); ++it){
		cout<<it->first<<it->second<<endl;
	}
}
int main( int argc, char* argv[] )
{
	char arr[3];
	arr[0] = 's';
	arr[1] = 'h';
 	char *arrptr = arr;
	if(*arrptr  == 's'){
		cout<<"eual"<<endl;
	}
	if(*(arrptr+1)  == 'h'){
		cout<<"eual"<<endl;
	}
	map<char, bool> flagbits;
	flagbits['O'] = false;
	flagbits['P'] = false;
	cout<<flagbits['O'] <<endl;
	traverseMap(flagbits) ;

string flagbit_str = "OPFS" ;

	for(int i = 0;i<flagbit_str.size(); i++)
	{
		cout<<flagbit_str[i]<<" ";

	}

	// unsigned int free:1;
	// cout << ~free << endl;
	cout<<"test bit "<<endl;
	PTE* p = new PTE();

	p-> present = p-> present ^ 1 ;

	cout<< p-> present <<endl;

}