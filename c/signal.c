#include "../h/xeroskernel.h"

/* Add code for the signal handler functions to this file. */


void sigtramp (void (*handler)(void *), void *cntx) {

    kprintf("#signal:SIGTRAMP");
    kprintf("#Handler to location %x", cntx);
    handler(cntx);

    kprintf("#syssigreturn to location %x", cntx);
    syssigreturn(cntx);

}

int signal(int pid, int sig_no){

    pcb *p;

    if (pid < 0) {
        return -1;
    }

    if (sig_no < 0 || sig_no > 31){
        return -2;
    }

    int i;
    for (i = 0; i < MAX_PROC; i++){
        if (proctab[i].pid == pid && proctab[i].state != STATE_STOPPED) {
            p = &proctab[i];
        }
    }

    if (p->state!=STATE_READY) { // process is blocked, so return address is the error code
        p->ret = -362;
        ready(p);
    }
    else
    {
        p->ret = sig_no; // process is active so can deliver the signal, hence sig_no
    }

    return 0;
}

int sigHandler(int sigNum, void (*new_handler)(void *), void (** old_handler)(void *), pcb *p) {

    kprintf("#signal:SIGNALHANDLER");
    kprintf("#signal:SIGNALHANDLER: signum = %d", sigNum);

    if (sigNum < 0 || sigNum > 31){
        return -1;
    }

    kprintf("#signal:SIGNALHANDLER:middle");
    if (!&new_handler) {
        return -2;
    }

    kprintf("#signal:SIGNALHANDLER:end");
    kprintf("#signal:SIGNALHANDLER:&new_handler = %d", &new_handler);
    *old_handler = p->signalblock[sigNum]; //save the old handler to old_handler
    p->signalblock[sigNum] = &new_handler; // set the new handler to the signalblock

    //TODO: remove magic number
    p->mask = 9;

    kprintf("#signal:signalhandler");
    return 0;
}