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

    // bool operator () ( Process* process1,  Process* process2) {
    //     if( process1->priority == process2->priority)
    //     {
    //         return  process1->enter_run_queue_time > process2->enter_run_queue_time ;
    //     }
    //         return process1->priority < process2->priority; //big first 
    // }    
    
    bool operator () ( Process* process1,  Process* process2) {
        if( process1->priority == process2->priority)
        {
            if(process1->enter_run_queue_time == process2->enter_run_queue_time){
            
/*            if( process1->prevState==CREATED &&process2->prevState==CREATED){
                return  process1->pid > process2->pid;
            }
            else if (process1->prevState==CREATED ||process2->prevState==CREATED){
                if(process1->prevState==CREATED&&process2->prevState!=CREATED)
                {
                    process2->rawtime += 100;
                }
                if( process1->prevState!=CREATED&&process2->prevState==CREATED )
                {
                    process1->rawtime += 100;
                }
                return  process1->rawtime > process2->rawtime;
            }*/


                return process1->order_proc > process2->order_proc;
            }
            return  process1->enter_run_queue_time > process2->enter_run_queue_time ;
        }
        return process1->priority < process2->priority; //big first 
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
struct cmpSJF //shortest job
{
    bool operator () ( Process* p1,  Process* p2) {
        if( p1->rem == p2->rem){
            if( p1->enter_run_queue_time == p2->enter_run_queue_time)
            {
                return p1->pid > p2->pid;
            }
            return  p1->enter_run_queue_time > p2->enter_run_queue_time ;
        }
        return p1->rem > p2->rem; 
    }

};
class SJF:public Scheduler{
    private:
    priority_queue<Process*, vector<Process*>, cmpSJF> rq2;
    public:
    SJF(string mode_input, int quantum_input):Scheduler(mode_input,quantum_input){}
    void add_process(Process* p){
        rq2.push(p);
    }
    Process* get_next_process(){
        if(rq2.empty())
        {
            return NULL;
            // if(rq2.empty()) { return NULL;}
        }
        Process* topproc = rq2.top(); 
        rq2.pop(); 
        return topproc ;
    }
};

struct cmpFCFS{
    bool operator () ( Process* p1,  Process* p2) {
        if( p1->enter_run_queue_time == p2->enter_run_queue_time)
        {
            return p1->rawtime = p2->rawtime;
            // return p1->pid > p2->pid;
        }
        return  p1->enter_run_queue_time > p2->enter_run_queue_time ;
    }
};
class FCFS: public Scheduler{
    private:
    // priority_queue<Process*, vector<Process*>, cmpFCFS> rq;
    queue<Process*> rq;
    public:
    FCFS(string mode_input, int quantum_input):Scheduler(mode_input,quantum_input){}
    void add_process(Process * proc){
        rq.push(proc) ;
    }
    Process* get_next_process(){
        if(rq.empty())
        {
            return NULL;
            // if(rq.empty()) { return NULL;}
        }
        Process* topproc = rq.front(); 
        rq.pop(); 
        return topproc ;
    }
};

class RR:public Scheduler{
    private:
    // priority_queue<Process*, vector<Process*>, cmpFCFS> rq3;
    queue<Process*> rq3;
    // queue<Process*> rq4;
    public:
    RR(string mode_input, int quantum_input):Scheduler(mode_input,quantum_input){}
    void add_process(Process * proc){
        rq3.push(proc) ;
    }
    Process* get_next_process(){
        if(rq3.empty())
        {
            return NULL;
            // swap(rq3,rq4) ;
            // if(rq3.empty()) { return NULL;}
        }
        Process* topproc = rq3.front(); 
        // Process* topproc = rq3.top(); 
        rq3.pop(); 
        return topproc ;
    }
    void dec_and_reset(Process* proc){
        rq3.push(proc) ; 
    }
     // when it is preempted, no new cpu burst, add to queue
}; 

#endif
