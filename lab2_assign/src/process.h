#ifndef __PROCESS__
#define __PROCESS__
#include <string>
#include <iostream>
#include <fstream>
#include "state.h"
using namespace std;


class Process{
 
    public:
        int pid, totalcpu, CB, IO ;
        int cb, ib;
        int rem;
        int priority, static_priority;
        bool need_new_cb ;
        STATE curState, prevState;
        int state_ts; // curState beginning timestamp
        int timeInPrevState; 
        Process(){}
/*
Process* pp = new Process(99, 9,9,9, Done, 3);
*/		Process(int pid, int state_ts, int totalcpu, int CB, int IO, STATE cur, int static_priority )
		:pid(pid),state_ts(state_ts), totalcpu(totalcpu), CB(CB), IO(IO) , curState(cur), static_priority(static_priority)
		{
            rem = totalcpu; //initialzed process rem
            priority = static_priority - 1;
            need_new_cb = true;
		}
        void reset_priority(){
            priority --;
            if(priority == -1){
                priority = static_priority - 1;// reset it
            }
                       
        }
        void printProc(){
            cout<<" cb="<<cb;
            cout<<" rem="<<rem; 
            cout<<" ib="<<ib;
            cout<<" prio="<<priority;

            // cout<<endl;
        }
};

#endif