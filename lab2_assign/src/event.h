#ifndef __EVENT__
#define __EVENT__
#include <time.h>
#include "process.h"
#include "state.h"
using namespace std;





class Event{
    public:
        int evtTimeStamp; //CURRENT_TIME = evtTimeStamp
        int genTimeStamp; //when to call new Event()
        Process* evtProcess;
        STATE oldstate, newstate; 
        //at evtTimeStamp, change from oldstate to newstate, and before this, 
        //stayed in oldstate(before transition) for span time  
        int span;
        TRANSITION transition; //event is a change of state

        time_t rawtime; //inner time telling which first enter queue
            
/*
Event * ee = new Event(99, Done, Done, TRANS_TO_PREEMPT, pp, 119);
*/
        Event(int timeStamp,int gen_time, STATE oldstate, STATE newstate, TRANSITION transition, Process* p)
        : evtTimeStamp(timeStamp),genTimeStamp(gen_time),oldstate(oldstate),newstate(newstate),transition(transition), evtProcess(p)
        { 
                time ( &rawtime );  
           }


};

#endif

