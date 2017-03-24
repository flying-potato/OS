#define PRIO (4)
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <sstream>
#include <string>
#include <cctype>
#include <iomanip>
#include <iostream>

#include "event.h"
#include "process.h"
#include "sched.h"
#include "state.h"
using namespace std;
struct mycmp
{

    bool operator () ( Event* evt1,  Event* evt2) {
        return evt1->evtTimeStamp > evt2->evtTimeStamp; //small ahead of big
    }

};


int CURRENT_TIME = 1;
bool CALL_SCHED = false;
Process * CURRENT_RUNNING_PROCESS = NULL;

vector<int> rand;
priority_queue<Event*, vector<Event*> , mycmp>  eventqueue; //time ordered 
// priority_queue<Event*>  eventqueue; //time ordered 
// queue<Event*> eventqueue ; 
bool get_event(Event* & e){
    if(eventqueue.empty()) return false;
    else{
        e = eventqueue.top();
        eventqueue.pop();
        return true;
    }
}

int get_next_event_time(){
    if(eventqueue.empty()) return -1;
    return eventqueue.top() ->evtTimeStamp;
}


void simulation(PrioSched& schedref);

int ofs = 0;
int randomPriority()
{
    int ret  =  rand[ofs] % PRIO ;
    ofs++;
    return ret ;    
}
int randomBurst(int burst){
    int ret = 1+rand[ofs]%burst ;
    ofs++ ;
    return  ret;  
}

void printEventQueue(){
    cout<<"--print current event queue--"<<endl;
    // priority_queue<Event*, vector<Event*> , mycmp>  copyeventqueue; //time ordered 
    while(!eventqueue.empty())
    {
        Event* e = eventqueue.top();
        cout<<"time="<<e->evtTimeStamp<<" "<< STA[e->oldstate] <<" -> "<<STA[e->newstate] <<endl;
        // cout<< e->evtTimeStamp <<" "<< e->evtProcess->pid << " " <<e->span<<": "
        // << STA[e->oldstate] <<" -> "<<STA[e->newstate] <<endl ;
        eventqueue.pop();
        // copyeventqueue.push(e);
    }
    // eventqueue = copyeventqueue
    cout<<"--end printing event queue--"<<endl;
}


int main( int argc, char* argv[] ){
    ifstream infile, rfile;
    int fileindex = 0;
    // infile.open(argv[fileindex + 1]);
    // rfile.open(argv[fileindex + 2]);
    infile.open("../input2");
    rfile.open("../rfile") ;
    //random file reading
    int numinline, randcount;
    rfile>>randcount;      
    while(rfile>>numinline){ 
        rand.push_back( numinline);
    }
    rfile.close();
/*    for(int i = 0; i<20; i++){
        cout<<rand[i]<<" "<<rand[i]%4<<" "<<rand[i]%90<<endl;
    }*/
    //event reading
    string line;
    stringstream linestream;
    int pid = 0;
    int arr_time, totalcpu, CB, IO;     
    PrioSched* sched = new PrioSched(); 
    cout<<"sched address: "<<sched<<endl;
    PrioSched& schedref = *sched;
    while( getline(infile,line) ){
        linestream.clear();
        linestream<<line;

        linestream>>arr_time;
        linestream>>totalcpu>>CB>>IO;

        Process *p = new Process(pid, arr_time, totalcpu, CB, IO, CREATED, randomBurst(PRIO));
        //static priority is  %PRIO+1, dynamic priority initialized from static-1
        // schedref.add_process(p);
        // cout<<"new Process addr: "<<p<<" "<<endl;
        Event* e = new Event(arr_time, CREATED,READY, TRANS_TO_READY, p );
        // cout<<"new Event addr: "<<e<<" "<<endl;
        eventqueue.push(e);

        pid ++; //next process
    }
    // test part
    // cout<<"shced runque top proc's prio: "<< (schedref.run_queue.top())->priority <<endl;
    cout<<"event num: "<<eventqueue.size()<<endl;
    infile.close();
    simulation(schedref);
    // printEventQueue();


    return 1;
}



void simulation(PrioSched& schedref){
	int quant = schedref.quantum;
    int cb;
    Event* evt;
	while (get_event(evt)){ //reference argument
        // cout<<"event timestam: "<<evt->evtTimeStamp<<endl;;
        CURRENT_TIME = evt->evtTimeStamp ; //change moment
		Process* & evtProc = evt->evtProcess;
        evtProc->timeInPrevState=CURRENT_TIME - evtProc->state_ts;
		int span = evtProc->timeInPrevState ; 
        // evtProc->prevState = evtProc->curState; 
		evtProc->curState = evt->newstate ; //process's new state
		evtProc->state_ts = CURRENT_TIME ; //update state_ts

        cout<<CURRENT_TIME<<" "<<evtProc->pid<<" "<< span<<": "<<STA[evt->oldstate] <<" -> "<<STA[evt->newstate];

        switch(evt->transition){
			//tell which old state come from
case TRANS_TO_READY: 	 //come from blocked, need new cb
            // evtProc->printProc() ;
            evtProc->need_new_cb = true;
            evtProc->priority = evtProc->static_priority - 1; //reset to highest dyn
            cout<<endl;
            // CREATED ->READY or BLOCK->READY
            /*	
            else{
                // Running to Ready : preemption, running finish so cb is the remain time
                if(evt->oldstate == RUNNG){ //keep running with old priority, at end priority--
                    evtProc->cb -= quant;	cout <<" cb="<< evtProc->cb;
                        // if rem < quantum ???, process at trans_to_run
                    evtProc->rem -= quant ; 	cout <<" rem="<< evtProc->rem <<endl;	
                    evtProc->priority --; 	cout<<" prio=" << evtProc->priority;
                    schedref.run_queue.push(evtProc) ; 
                // Blocked to Ready, when blocked, no cb or rem changed
                }else{
                    schedref.run_queue.push(evtProc) ; 
                }
            }*/
            
            schedref.run_queue.push(evtProc) ; //add process to run_queue
            // cout<<"run_quuu size "<<schedref.run_queue.size()<<endl;
            CALL_SCHED = true; //conditional on whether something is run
            break;

			// when trans to running, we need give the cb,
	
case TRANS_TO_RUN: 
        //ready -> run //when it has cb from preemption
            // no need to create new cb
            if (evtProc->need_new_cb){
                cb = randomBurst( evtProc->CB);
            }else{ 
                cb = evtProc->cb; // no need for new cb
            }

            if (evtProc->rem <= quant && cb > evtProc->rem) { //****last run 
                cb = evtProc->rem;
                cout <<" cb="<<cb<<" rem="<< evtProc->rem  <<" prio="<< evtProc->priority <<endl;
                cout << CURRENT_TIME+cb<< " " << evtProc->pid << " "<<cb<<": Done"<<endl;
                CURRENT_RUNNING_PROCESS = NULL;
                CALL_SCHED = true;// WHEN DONE, need CALL SCHED
                break;
            } 
            evtProc->cb = cb;
            cout <<" cb="<<cb<<" rem="<< evtProc->rem  <<" prio="<< evtProc->priority <<endl;
            Event* ee ; 
            //begin to run then create event at the end of running, add to eventqueue
            if (cb <= quant){ // keep prio, go to block 
            //evt_ts is at the end of running, namely the next transition
                ee = new  Event(CURRENT_TIME+cb, RUNNG, BLOCK, TRANS_TO_BLOCK, evtProc);
            }else{ //go to ready, decrease priority by 1 after running
                //only know RUNNING to READY, when it TRANS_TO_READY calculate the rem and remaning cb, decrease prio
                ee = new Event(CURRENT_TIME+quant, RUNNG, READY, TRANS_TO_PREEMPT, evtProc);
            }
            eventqueue.push(ee) ;
            break;

case TRANS_TO_BLOCK:
            //RUNNING-> BLOCKED, cb must small than quant, make cb become -1
            CURRENT_RUNNING_PROCESS = NULL ; //running ended
            evtProc->rem -= evtProc->cb; //related with running time, cb<=quantum
            evtProc->ib = randomBurst(evtProc->IO) ;
            //create an event for when process becomes READY again
            cout<<" ib="<< evtProc->ib <<" rem="<<evtProc->rem<<endl;
            ee = new  Event(CURRENT_TIME+evtProc->ib, BLOCK, READY, TRANS_TO_READY, evtProc);
            eventqueue.push(ee) ;
            CALL_SCHED = true;
            break;

case TRANS_TO_PREEMPT: 
        //RUNING -> READY cb=2 rem=15 prio=1, cb > quant
            // add to runqueue (no event is generated), 

            CURRENT_RUNNING_PROCESS = NULL ; //running ended
             //keep running with old priority, at end priority--
            evtProc->need_new_cb = false; //no need to create new cb
            evtProc->cb -= quant;	
            cout <<"  cb="<< evtProc->cb;
                // if rem < quantum ???, process at trans_to_run

            evtProc->rem -= quant ; 	
            cout <<" rem="<< evtProc->rem;	

            cout<<" prio=" << evtProc->priority<<endl;
            evtProc->reset_priority() ; // -- operation
            //first print then cout prio
            schedref.run_queue.push(evtProc) ; 
            // Blocked to Ready, when blocked, no cb or rem changed

            
            CALL_SCHED = true;
            break;
		
    	}
		
        // cout<<"run queue top process: " <<schedref.run_queue.top()->pid<<endl;
		delete[] evt;
        // cout<<"eventqueue size: "<<eventqueue.size()<<endl;
		if(CALL_SCHED){
			if( get_next_event_time() == CURRENT_TIME ){

				continue;
			}
			CALL_SCHED = false;
			if(	CURRENT_RUNNING_PROCESS == NULL) // can run a new process
			{
				CURRENT_RUNNING_PROCESS = schedref.get_next_process();
				if(CURRENT_RUNNING_PROCESS == NULL){ continue ;}
                else{
                    Event* run_event = new Event(CURRENT_TIME, READY, RUNNG, TRANS_TO_RUN, CURRENT_RUNNING_PROCESS) ;
                    eventqueue.push(run_event) ;
                }
			}
			// create event to make process runnable for smae time

		}
	}//while loop
}
