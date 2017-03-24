void simulation(){
	int quant = schedref.quantum;
    Event* evt;
	while (get_event(evt)){
        CURRENT_TIME = evt->evtTimeStamp ; //change moment
		Process* evtProc = evt->evtProcess;
        evtProc->timeInPrevState=CURRENT_TIME - evtProc->state_ts;
		int span = evtProc->timeInPrevState ; 

		evtProc->curState = evt->newstate ; //process's new state
		evtProc->state_ts = CURRENT_TIME ; //update state_ts

        cout<<CURRENT_TIME<<" "<<evtProc->pid<<" "<< span<<" "<<": "<<STA[evt->oldstate] <<" -> "<<STA[evt->newstate];

        switch(evt->transition){
			//tell which old state come from
            case TRANS_TO_READY: 	
				/*	
				if(evt->oldstate == CREATED){
					// do nothing
					schedref.run_queue.push(evtProc) ; 
				}
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
				schedref.run_queue.push(evtProc) ; 
				CALL_SCHED = true; //conditional on whether something is run
				break;

			// when trans to running, we need give the cb,
			// we know when the process ended, (quantum expired or cb over or Done,
				//Done is rem < min(cb, quantum)
			// create an  or RUN->BLOCK event, 
			// if RUN-> READY, decrease the  priority when quantum expired 
			// if RUN -> BLOCK event, 
			case TRANS_TO_RUN: //run -> blocked or run-> ready
				// create event for either preemption or blocking
				int cb = randomBurst( evtProc->CB); //may argument is CB or rem
				if (evtProc->rem <= quant && cb > evtProc->rem) { //****last run 
					cb = evtProc->rem;
					cout <<" cb="<<cb<<" rem="<< evtProc->rem  <<" prio="<< evtProc->priority <<endl;
					cout << CURRENT_TIME+cb<< " " << evtProc->pid << " "<<cb<<": Done"<<endl;
				} 
				evtProc->cb = cb;
				cout <<" cb="<<cb<<" rem="<< evtProc->rem  <<" prio="<< evtProc->priority <<endl;
				
				//begin to run then create event at the end of running, add to eventqueue
				if (cb <= quant){ // keep prio, go to block 
				//evt_ts is at the end of running, namely the next transition
					
					Event* ee = new  Event(CURRENT_TIME+cb, RUNNG, BLOCK, TRANS_TO_BLOCK, evtProc);
				}else{ //go to ready, decrease priority by 1 after running
					//only know RUNNING to READY, when it TRANS_TO_READY calculate the rem and remaning cb, decrease prio
					Event* ee = new Event(CURRENT_TIME+quant, RUNNG, READY, TRANS_TO_PREEMPT, evtProc);
				}
				eventqueue.push(ee) ;
				break;
			case TRANS_TO_BLOCK: //RUNNING-> BLOCKED, cb must small than quant
				//can know next trans to READY
				evtProc->rem -= evtProc->cb; //related with running time, cb<=quantum
				evtProc->ib = randomBurst(evtProc->IO) ;
				//create an event for when process becomes READY again
				cout<<" ib="<< evtProc->ib <<" rem="<<evtProc->rem<<end;
				Event* ee = new  Event(CURRENT_TIME+evtProc->ib, BLOCK, READY, TRANS_TO_READY, evtProc);
				eventqueue.push(ee) ;
				CALL_SCHEDULER = true;
				break;
			case TRANS_TO_PREEMPT: //RUNING -> READY cb=2 rem=15 prio=1, cb > quant
				// add to runqueue (no event is generated), 
				
				if(evt->oldstate == RUNNG){ //keep running with old priority, at end priority--
					evtProc->cb -= quant;	cout <<" cb="<< evtProc->cb;
						// if rem < quantum ???, process at trans_to_run
					evtProc->rem -= quant ; 	cout <<" rem="<< evtProc->rem <<endl;	
					evtProc->priority --; 	cout<<" prio=" << evtProc->priority;
					schedref.run_queue.push(evtProc) ; 
					// Blocked to Ready, when blocked, no cb or rem changed

				}
				CALL_SCHEDULER = true;
				break;
		
    	}
		
	
		delete[] evt;
		if(CALL_SCHED){
			if( get_next_event_time() == CURRENT_TIME ){
				continue;
			}
			CALL_SCHED = false;
			if(	CURRENT_RUNNING_PROCESS = NULL)
			{
				CURRENT_RUNNING_PROCESS = schedref.get_next_process();
				if(CURRENT_RUNNING_PROCESS == NULL){ continue ;}
			}
			// create event to make process runnable for smae time

		}
	}//while loop
}
