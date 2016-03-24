/* disp.c : dispatcher
 */

//#include <xeroskernel.h>
//#include <i386.h>
//#include <xeroslib.h>
#include <stdarg.h>
#include "../h/xeroskernel.h"
#include "../h/i386.h"
#include "../h/xeroslib.h"

static pcb      *head = NULL;
static pcb      *tail = NULL;

void     dispatch( void ) {
/********************************/

    pcb         *p;
    int         r;
    funcptr     fp;
    int         stack;
    va_list     ap;
    char        *str;
    int         len;
    void*       oldHandler;
    void*       newHandler;

    for( p = next(); p; ) {
        //      kprintf("Process %x selected stck %x\n", p, p->esp);

        r = contextswitch( p );
        switch( r ) {
            case( SYS_CREATE ):
                ap = (va_list)p->args;
                fp = (funcptr)(va_arg( ap, int ) );
                stack = va_arg( ap, int );
                p->ret = create( fp, stack );
                break;
            case( SYS_YIELD ):
                ready( p );
                p = next();
                break;
            case( SYS_STOP ):
                p->state = STATE_STOPPED;
                p = next();
                break;
            case( SYS_PUTS ):
                ap = (va_list)p->args;
                str = va_arg( ap, char * );
                kprintf( "%s", str );
                p->ret = 0;
                break;
            case( SYS_GETPID ):
                p->ret = p->pid;
                break;
            case( SYS_SLEEP ):
                ap = (va_list)p->args;
                len = va_arg( ap, int );
                sleep( p, len );
                p = next();
                break;
            case( SYS_TIMER ):
                tick();
                ready( p );
                p = next();
                end_of_intr();
                break;
            case( SYS_SIGHANDLER ):
                kprintf("#disp:SYS_SIGHANDLER");
                ap = (va_list)p->args;
                int sig = va_arg(ap, int );
                newHandler = va_arg( ap, void *);
                oldHandler = va_arg( ap, void *);
                int returnNum = sigHandler(sig, newHandler, oldHandler, p);
                kprintf("#disp:SYS_SIGHANDLER:returnNumber = %d", returnNum);
                p->ret = returnNum;
                break;
            case( SYS_SIGRETURN ):
                kprintf("#disp:SYS_SIGRETURN");
                ap = (va_list)p->args;
                void *old_sp = va_arg( ap, void* );
                p->args = old_sp;
                break;
            case( SYS_KILL ):
                kprintf("#disp:SYS_KILL");
                ap = (va_list)p->args;
                int pid = va_arg( ap, int );
                int sigNumber = va_arg( ap, int );
                int s = signal(pid, sigNumber);

                if (s == -1) {
                    // invalid pid
                    p->ret = -712;
                } else if (s == -2) {
                    // invalid signal
                    p->ret = -651;
                } else {
                    p->ret = 0;
                }
                break;
           /* when open is called the FDT is set up. It happens here because we have access to the process and no other
            * operation can happen unless you've opened a device (ie you can't close unless you've opened).
            */
            case ( OPEN ):
                ap = (va_list) p->args;
                int device_no = va_arg(ap, int);
                p->ret = di_open(device_no, p);
                break;
            case ( CLOSE ):
                ap = (va_list) p->args;
                int fd = va_arg(ap, int);
                p->ret = di_close(fd, p);
                break;
            case ( WRITE ):
                ap = (va_list) p->args;
                int writefd = va_arg(ap, int);
                void *buff = va_arg(ap, void*);
                int bufflen = va_arg(ap, int);
                p->ret = di_write(writefd, buff, bufflen, p);
                break;
            case ( READ ):
                ap = (va_list) p->args;
                int readfd = va_arg(ap, int);
                void *buff_read = va_arg(ap, void*);
                int bufflen_read = va_arg(ap, int);
                p->ret = di_read(readfd,buff_read,bufflen_read,p);
                break;
            case ( IOCTL ):
                ap = (va_list) p->args;
                int ioctl_fd = va_arg(ap, int);
                unsigned long command = va_arg(ap, unsigned long);
                int ioctl_args = va_arg(ap, int);
                p->ret = di_ioctl(ioctl_fd, command, ioctl_args, p);
                break;

            default:
                kprintf( "Bad Sys request %d, pid = %d\n", r, p->pid );
                break;
        }
    }

    kprintf( "Out of processes: dying\n" );

    for( ;; );
}

extern void dispatchinit( void ) {
/********************************/

    //bzero( proctab, sizeof( pcb ) * MAX_PROC );
    memset(proctab, 0, sizeof( pcb ) * MAX_PROC);
}



extern void     ready( pcb *p ) {
/*******************************/

    p->next = NULL;
    p->state = STATE_READY;

    if( tail ) {
        tail->next = p;
    } else {
        head = p;
    }

    tail = p;
}

extern pcb      *next( void ) {
/*****************************/

    pcb *p;

    p = head;

    if( p ) {
        head = p->next;
        if( !head ) {
            tail = NULL;
        }
    } else {
        kprintf( "BAD\n" );
        for(;;);
    }

    p->next = NULL;
    p->prev = NULL;
    checksignal(p);
    return( p );
}


extern pcb *findPCB( int pid ) {
/******************************/

    int	i;

    for( i = 0; i < MAX_PROC; i++ ) {
        if( proctab[i].pid == pid ) {
            return( &proctab[i] );
        }
    }

    return( NULL );
}


/* This runs before next() returns the pcb so we can check if there are any interrupts
 * pending for this process to execute.
 *
 * @param the pcb of this process
 *
 * @return call to sigtramp the &handler and context_frame
 * */
void checksignal(pcb *p) {
//    kprintf("disp:CHECKSIGNAL");

    unsigned int s;
    int sig_no;
    s = p->mask;
    sig_no = 0;
//    kprintf("before s");
    if (s == 0) {
        return;
    }
//    kprintf("after s");:


//    while (s >>= 1) {
//        sig_no++;
//    }
    sig_no = 9;

//    p->mask &= ~(1 << sig_no);

    kprintf("disp:CHECKSIGNAL:before");
    sigtramp((void (*)(void *)) p->signalblock[sig_no], p->ctx_frm);
    kprintf("disp:CHECKSIGNAL:after");
}

