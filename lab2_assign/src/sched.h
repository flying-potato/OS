#ifndef __SCHED__
#define __SCHED__
#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
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

struct mycmpProcessSJF //shortest job
{
    bool operator () ( Process* process1,  Process* process2) {
        if( process1->rem == process2->rem)
        {
            return  process1->enter_run_queue_time > process2->enter_run_queue_time ;
        }
        return process1->rem > process2->rem; 
    }

};

class Scheduler{
    protected: //visited by child class
    string mode;
    int quantum;
    priority_queue<Process*, vector<Process*>, mycmpProcess> run_queue; //all ready state process here
    priority_queue<Process*, vector<Process*>, mycmpProcess> run_queue_expired; 

    public:

    Scheduler(string mode_input, int quantum_input){ 
        mode = mode_input;
        quantum = quantum_input;    
    }
    virtual string get_mode(){
        return mode;
    }
    virtual int get_quantum(){
        return quantum;
    }
    virtual void add_process(Process * proc){
        run_queue.push(proc) ;
    }
    virtual Process* get_next_process(){
        if(run_queue.empty())
        {
            return NULL;
            // if(run_queue.empty()) { return NULL;}
        }
        Process* topproc = run_queue.top(); 
        run_queue.pop(); 
        return topproc ;
    }
    virtual void dec_and_reset(Process* proc){ }
    virtual ~Scheduler(){} //delete[] 
};

class PrioSched: public Scheduler{
    public:
    PrioSched(string mode_input, int quantum_input):Scheduler(mode_input,quantum_input){}

    Process* get_next_process(){
        if(run_queue.empty())
        {
            swap(run_queue, run_queue_expired) ; //queue member function swap
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
class FCFS: public Scheduler{
    public:
    FCFS(string mode_input, int quantum_input):Scheduler(mode_input,quantum_input){}
};
class LCFS: public Scheduler{
    private:
    stack<Process*> run_stack ;
    public:
    LCFS(string mode_input, int quantum_input):Scheduler(mode_input,quantum_input){}

    void add_process(Process* proc){
        run_stack.push(proc) ;
    }

    Process* get_next_process(){
        if(run_stack.empty())
        {
            return NULL;
            // if(run_stack.empty()) { return NULL;}
        }
        Process* topproc = run_stack.top(); 
        run_stack.pop(); 
        return topproc ;
    }
};
class ShortestJobFirst:public Scheduler{
    private:
    priority_queue<Process*, vector<Process*>, mycmpProcessSJF> run_queue2;
    public:
    ShortestJobFirst(string mode_input, int quantum_input):Scheduler(mode_input,quantum_input){}


};

#endif
