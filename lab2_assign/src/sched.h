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
        if( process1->priority == process2->priority)
        {
            return  process1->enter_run_queue_time > process2->enter_run_queue_time ;
        }
            return process1->priority < process2->priority; //big first 
    }

};


class PrioSched{
    public:
    string mode= "PRIO";
    int quantum = 2;
    
    priority_queue<Process*, vector<Process*>, mycmpProcess> run_queue; //all ready state process here
    priority_queue<Process*, vector<Process*>, mycmpProcess> run_queue_expired; 
    PrioSched(){}
    void exchange(){
        swap(run_queue, run_queue_expired) ;
    }
    void add_process(Process * proc){
        run_queue.push(proc) ;
    }

    Process* get_next_process(){
        if(run_queue.empty())
        {
            swap(run_queue, run_queue_expired) ;
            if(run_queue.empty()) { return NULL;}
        }
        Process* topproc = run_queue.top(); 
        run_queue.pop(); 
        // cout<<"getting:" <<topproc->pid <<";top: "<<run_queue.top()->pid <<endl;

        return topproc ;
    }
    void dec_and_reset(Process* proc){

        proc->priority --;
        if(proc->priority == -1){
            proc->priority = proc->static_priority - 1;// reset it and push to expired
            run_queue_expired.push(proc) ; 
        }
        else{ run_queue.push(proc) ; }
    }

};

#endif
