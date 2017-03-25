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
#include <map>
#include <unistd.h>

#include "event.h"
#include "process.h"
#include "sched.h"
#include "state.h"
using namespace std;
struct mycmp
{
    //big value in < expression pop out first
    bool operator () ( Event* evt1,  Event* evt2) { //similar to reload <
        if(evt1->evtTimeStamp == evt2->evtTimeStamp){
            return evt1->genTimeStamp > evt2->genTimeStamp;
        }
        return evt1->evtTimeStamp > evt2->evtTimeStamp; //small ahead of big
    }

};
struct mycmpfinish
{
    //big value in < expression pop out first
    bool operator () ( Process* proc1,  Process* proc2) { //similar to reload <
        return proc1->pid > proc2->pid; //small ahead of big
    }
};
int quant;
int CURRENT_TIME = 0;
int PROC_NUM = 0;
bool CALL_SCHED = false;
Process * CURRENT_RUNNING_PROCESS = NULL;
bool if_IO=false; 
int BLOCK_START=0, BLOCK_END=0, IO_TIME=0;
vector<int> rand;
priority_queue<Process*, vector<Process*>, mycmpfinish> finish_queue;
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


void simulation(Scheduler& schedref);

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
    char tmp;
    int vflag = 0;
    char* sched_option = NULL;
    char* infile_name = NULL;
    char* randfile_name = NULL;
    while((tmp=getopt(argc,argv,"vs:"))!=-1)
    {
        switch(tmp){
        case 'v':
            vflag = 1 ;
            break;
        case 's':
            sched_option = optarg;
            break; 
        }
    }
    if (optind < argc){
        infile_name = argv[optind];
        optind++;
        randfile_name = argv[optind];
    }   
    Scheduler* sched;
    int q = sched_option[1]-48; 
    switch(sched_option[0]){
        case 'P':
            sched = new PrioSched("PRIO", q); 
            break;
        case 'F':
            sched = new FCFS("FCFS", 10000);
            break;
        case 'L':
            sched = new LCFS("LCFS", 10000);
            break;
        case 'S':
            sched = new SJF("SJF", 10000);
            break;
        case 'R':
            sched = new RR("RR", q);
            break;
            
    }
    //read option and filename
    ifstream infile, rfile;
    // infile.open("../input1"); //input#
    // rfile.open("../rfile") ;
    infile.open(infile_name);
    rfile.open(randfile_name);
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
    // PrioSched* sched = new PrioSched(); 
    
    Scheduler& schedref = *sched;
    while( getline(infile,line) ){
        linestream.clear();
        linestream<<line;

        linestream>>arr_time;
        linestream>>totalcpu>>CB>>IO;

        Process *p = new Process(pid, arr_time, totalcpu, CB, IO, CREATED, randomBurst(PRIO));
        //static priority is  %PRIO+1, dynamic priority initialized from static-1
        Event* e = new Event(arr_time,arr_time, CREATED,READY, TRANS_TO_READY, p );

        eventqueue.push(e);
        pid ++; //next process
    }

    infile.close();
    simulation(schedref);



    return 1;
}



void simulation(Scheduler& schedref){
	quant = schedref.get_quantum();
    int cb;
    Event* evt;
	while (get_event(evt)){ //reference argument
        // cout<<"event timestam: "<<evt->evtTimeStamp<<endl;;
        CURRENT_TIME = evt->evtTimeStamp ; //change moment
		Process* & evtProc = evt->evtProcess;
        evtProc->timeInPrevState=CURRENT_TIME - evtProc->state_ts;
		int span = evtProc->timeInPrevState ; 
        if(evt->oldstate == READY) {evtProc->CW += span;} //cpu waiting time
        // evtProc->prevState = evtProc->curState; 
		evtProc->curState = evt->newstate ; //process's new state
		evtProc->state_ts = CURRENT_TIME ; //update state_ts

        // cout<<CURRENT_TIME<<" "<<evtProc->pid<<" "<< span<<": "<<STA[evt->oldstate] <<" -> "<<STA[evt->newstate];
        cout<<CURRENT_TIME<<" "<<evtProc->pid<<" "<< span<<": " ;
        if(evt->newstate == Done){
            cout<<"Done"<<endl;
        }else{
            cout<<STA[evt->oldstate] <<" -> "<<STA[evt->newstate];
        }     

        switch(evt->transition){
			//tell which old state come from
case TRANS_TO_READY: 	// CREATED ->READY or BLOCK->READY or 
//RUNNING to READY TRANS_TO_PREEMPT
            // evtProc->printProc() ;
            if(evt->oldstate == CREATED) {
                // cout<<" prio="<<evtProc->priority;
                evtProc->need_new_cb = true;
                PROC_NUM ++;
            }
            else{ //BLOCKED
                evtProc->priority = evtProc->static_priority - 1; //reset to highest dyn
                evtProc->need_new_cb = true;
            }
            evtProc->enter_run_queue_time = CURRENT_TIME;      
            // cout<<" rdy:"<<evtProc->enter_run_queue_time
            cout<<endl;   
            schedref.add_process(evtProc) ; //add process to run_queue
            // cout<<"run_quuu size "<<schedref.run_queue.size()<<endl;
            CALL_SCHED = true; //conditional on whether something is run
            break;

			// when trans to running, we need give the cb,
case TRANS_TO_DONE:
            evtProc->finish(CURRENT_TIME);
            finish_queue.push(evtProc) ; // finish proc
            CURRENT_RUNNING_PROCESS = NULL;
            CALL_SCHED = true;// WHEN DONE, need CALL SCHED       
            break; 	
case TRANS_TO_RUN: 
        //ready -> run //when it has cb from preemption
            // no need to create new cb

            if (evtProc->need_new_cb){
                cb = randomBurst( evtProc->CB);
            }else{ 
                cb = evtProc->cb; // no need for new cb
            }

            if (evtProc->rem <= quant && cb >= evtProc->rem) { //done event
                cb = evtProc->rem;
                // in RUNING
                cout <<" cb="<<cb<<" rem="<< evtProc->rem  <<" prio="<< evtProc->priority <<endl; 

                // int FT = CURRENT_TIME+cb;
                // evtProc->state_ts = FT;
                // cout << FT<< " " << evtProc->pid << " "<<cb<<": Done"<<endl;
                // evtProc->finish(FT);
                // finish_queue.push(evtProc) ; // finish proc

                // CURRENT_TIME = FT;
                // CURRENT_RUNNING_PROCESS = NULL;
                // CALL_SCHED = true;// WHEN DONE, need CALL SCHED

                Event* doneEvt = new Event(CURRENT_TIME+cb, CURRENT_TIME, RUNNG, Done, TRANS_TO_DONE, evtProc);
                eventqueue.push(doneEvt);
                break;
            } 
            evtProc->cb = cb;
            cout <<" cb="<<cb<<" rem="<< evtProc->rem  <<" prio="<< evtProc->priority <<endl;
            Event* ee ; 
            //begin to run then create event at the end of running, add to eventqueue
            if (cb <= quant){ // keep prio, go to block 
            //evt_ts is at the end of running, namely the next transition
                evtProc->need_new_cb = true;
                ee = new Event(CURRENT_TIME+cb, CURRENT_TIME, RUNNG, BLOCK, TRANS_TO_BLOCK, evtProc);
            }else{ //go to ready, decrease priority by 1 after running
                //only know RUNNING to READY, when it TRANS_TO_READY calculate the rem and remaning cb, decrease prio
                ee = new Event(CURRENT_TIME+quant, CURRENT_TIME, RUNNG, READY, TRANS_TO_PREEMPT, evtProc);
            }
            eventqueue.push(ee) ;
            break;

case TRANS_TO_BLOCK:
            //RUNNING-> BLOCKED, cb must small than quant, make cb become -1
            
            CURRENT_RUNNING_PROCESS = NULL ; //running ended
            evtProc->rem -= evtProc->cb; //related with running time, cb<=quantum
            evtProc->ib = randomBurst(evtProc->IO) ;
            //create an event for when process becomes READY again
            cout<<"  ib="<< evtProc->ib <<" rem="<<evtProc->rem<<endl;
            evtProc->IT += evtProc->ib;
            ee = new  Event(CURRENT_TIME+evtProc->ib, CURRENT_TIME, BLOCK, READY, TRANS_TO_READY, evtProc);

            if(if_IO == false || CURRENT_TIME >= BLOCK_END){ //easy just plus the ib
                if_IO = true;
                IO_TIME += evtProc->ib;
                BLOCK_START = CURRENT_TIME;
                BLOCK_END = CURRENT_TIME + evtProc->ib ;
            }else{
                if(CURRENT_TIME+ evtProc->ib <= BLOCK_END){}
                else{
                    int extra_IO =  CURRENT_TIME+ evtProc->ib - BLOCK_END;
                    IO_TIME += extra_IO ;
                    BLOCK_START = CURRENT_TIME;
                    BLOCK_END = CURRENT_TIME+ evtProc->ib ;
                }
            }

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

            evtProc->enter_run_queue_time = CURRENT_TIME; 
            schedref.dec_and_reset(evtProc);
/*            evtProc->priority --;
            if(evtProc->priority == -1){
                evtProc->priority = evtProc->static_priority - 1;// reset it and push to expired
                schedref.run_queue_expired.push(evtProc) ; 
            }
            else{ schedref.run_queue.push(evtProc) ; }*/
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
                    Event* run_event = new Event(CURRENT_TIME, CURRENT_TIME, READY, RUNNG, TRANS_TO_RUN, CURRENT_RUNNING_PROCESS) ;
                    eventqueue.push(run_event) ;
                }
			}
			// create event to make process runnable for smae time

		}
	}//while loop
    int last_FT = -1;
    double cpu_util, io_util, total_CPU, throughput, av_tt, av_cw;
    if(schedref.get_mode() == "RR" || schedref.get_mode()=="PRIO"){
        printf("%s %d\n", schedref.get_mode().c_str(), quant);
    }
    else{ printf("%s\n", schedref.get_mode().c_str()); }
    while (!finish_queue.empty())
    {
        Process &pt = *finish_queue.top();
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
            pt.pid,
            pt.arrival, pt.totalcpu, pt.CB, pt.IO, pt.static_priority,
            pt.state_ts, // last time stamp
            pt.TT,
            pt.IT,
            pt.CW); 
        last_FT = max(last_FT, pt.FT) ;
        total_CPU += pt.totalcpu;
        av_tt += pt.TT ;
        av_cw += pt.CW ;
        finish_queue.pop();
    }
    cpu_util = total_CPU/last_FT *100;
    // cout<<"IO_TIME:"<<IO_TIME<<endl;
    io_util = double(IO_TIME* 100.0 / last_FT  ) ;
    throughput = (PROC_NUM*100.0)/last_FT ;
    av_tt  /= PROC_NUM ;
    av_cw  /= PROC_NUM ;
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
        last_FT, cpu_util , io_util, av_tt, av_cw, throughput);

}
