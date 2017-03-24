#ifndef __SCHED__
#define __SCHED__
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <sstream>
#include <string>
#include <cctype>
#include <iomanip>

#include "process.h"

struct mycmpProcess
{

    bool operator () ( Process* process1,  Process* process2) {
/*        if (process1->priority == process2->priority)
        {
            return 
        }
*/        return process1->priority < process2->priority; //big first 
    }

};


class PrioSched{
    public:
    int quantum = 2;
    priority_queue<Process*, vector<Process*>, mycmpProcess> run_queue; //all ready state process here
    priority_queue<Process*, vector<Process*>, mycmpProcess> run_queue_expired; 

    PrioSched(){}

    void add_process(Process * proc){
        run_queue.push(proc) ;
    }

    Process* get_next_process(){
        if(run_queue.empty())
        {
            return NULL;
        }
        Process* topproc = run_queue.top(); 
        run_queue.pop(); 
        cout<<"getting:" <<topproc->pid <<";top: "<<run_queue.top()->pid <<endl;

        return topproc ;
    }
    
};

#endif
