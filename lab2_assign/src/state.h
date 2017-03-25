#ifndef __STATE__
#define __STATE__

using namespace std;
enum TRANSITION{
    TRANS_TO_READY,TRANS_TO_RUN,
    TRANS_TO_BLOCK, TRANS_TO_PREEMPT, TRANS_TO_DONE
};
enum STATE{
    CREATED, READY, RUNNG, BLOCK, Done
};
string TRAN[4] ={ "TRANS_TO_READY","TRANS_TO_RUN", "TRANS_TO_BLOCK", "TRANS_TO_PREEMPT"};
string STA[5] ={"CREATED", "READY", "RUNNG", "BLOCK", "Done"};
#endif


