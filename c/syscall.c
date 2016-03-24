/* syscall.c : syscalls
 */

//#include <xeroskernel.h>
#include <stdarg.h>
#include "../h/xeroskernel.h"


void debug(char *bmsg, int bnum);


int syscall(int req, ... ) {
/**********************************/

    va_list     ap;
    int         rc;

    va_start( ap, req );

    __asm __volatile( " \
        movl %1, %%eax \n\
        movl %2, %%edx \n\
        int  %3 \n\
        movl %%eax, %0 \n\
        "
        : "=g" (rc)
        : "g" (req), "g" (ap), "i" (KERNEL_INT)
        : "%eax" 
    );
 
    va_end( ap );

    return( rc );
}

 int syscreate( funcptr fp, int stack ) {
/*********************************************/

    return( syscall( SYS_CREATE, fp, stack ) );
}

 int sysyield( void ) {
/***************************/

    return( syscall( SYS_YIELD ) );
}

 int sysstop( void ) {
/**************************/

    return( syscall( SYS_STOP ) );
}

int sysgetpid( void ) {
/****************************/

    return( syscall( SYS_GETPID ) );
}

void sysputs( char *str ) {
/********************************/

    syscall( SYS_PUTS, str );
}

unsigned int syssleep( unsigned int t ) {
/*****************************/

    return syscall( SYS_SLEEP, t );
}

void syssigreturn(void *old_sp) {
    kprintf("#syscall:SYSSIGRETURN");

    debug("test", 808); //testing if debug works

    syscall(SYS_SIGRETURN, old_sp);
}

int syssighandler(int signal, void (*newhandler)(void *), void (** oldHandler)(void *)) {
    kprintf("#syscall:SYSSIGHANDLER");

    return( syscall(SYS_SIGHANDLER, signal, newhandler, oldHandler) );
}

int syskill(int pid, int signalNumber) {
     kprintf("#syscall:SYSKILL");

    return( syscall(SYS_KILL, pid, signalNumber) );
}

extern int sysopen(int device_no) {
    return syscall(OPEN, device_no);
}

extern int sysclose(int fd) {
    return syscall(CLOSE, fd);
}

extern int syswrite(int fd, void *buff, int bufflen) {
    return syscall(WRITE, fd, buff, bufflen);
}

extern int sysread(int fd, void *buff, int bufflen) {
    return syscall(READ, fd, buff, bufflen);
}

extern int sysioctl(int fd, unsigned long command, ...) {
    int ret;
    va_list ap;
    va_start(ap, command);

    return ret = syscall(IOCTL, fd, command, ap);
    va_end(ap);
}

void debug(char *bmsg, int bnum) { kprintf("\nBUG: %s %d\n", bmsg, bnum); }

