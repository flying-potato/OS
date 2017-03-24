#ifndef __EVENT__
#define __EVENT__
#include "process.h"
#include "state.h"
using namespace std;





class Event{
    public:
        int evtTimeStamp; //CURRENT_TIME = evtTimeStamp
        Process* evtProcess;
        STATE oldstate, newstate; 
        //at evtTimeStamp, change from oldstate to newstate, and before this, 
        //stayed in oldstate(before transition) for span time  
        int span;
        TRANSITION transition; //event is a change of state

        Event(int arrtime):evtTimeStamp(arrtime){ }
/*
Event * ee = new Event(99, Done, Done, TRANS_TO_PREEMPT, pp, 119);
*/
        Event(int timeStamp, STATE oldstate, STATE newstate, TRANSITION transition, Process* p)
        : evtTimeStamp(timeStamp),oldstate(oldstate),newstate(newstate),transition(transition), evtProcess(p)
        {    }


};

// bool operator < ( Event* evt1,  Event* evt2) {
//     return evt2->evtTimeStamp < evt1->evtTimeStamp;
// }

// class ComparisonClass {
//     bool operator() ( Event* evt1,  Event* evt2) {
//         //comparison code here
//         return evt2->evtTimeStamp < evt1->evtTimeStamp;
//     }
// };

#endif

