/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max */

//#include <sys/_types/_dev_t.h>

/* Symbolic constants used throughout Xinu */

typedef	char    Bool;   /* Boolean type                  */
#define	FALSE   0       /* Boolean constants             */
#define	TRUE    1
#define	EMPTY   (-1)    /* an illegal gpq                */
#define	NULL    0       /* Null pointer for linked lists */
#define	NULLCH '\0'     /* The null character            */


/* Universal return constants */

#define	OK            1         /* system call ok               */
#define	SYSERR       -1         /* system call failed           */
#define	EOF          -2         /* End-of-file (usu. from read)	*/
#define	TIMEOUT      -3         /* time out  (usu. recvtim)     */
#define	INTRMSG      -4         /* keyboard "intr" key pressed	*/
                                /*  (usu. defined as ^B)        */
#define	BLOCKERR     -5         /* non-blocking op would block  */

/* Functions defined by startup code */


void           bzero(void *base, int cnt);
void           bcopy(const void *src, void *dest, unsigned int n);
void           disable(void);
unsigned short getCS(void);
unsigned char  inb(unsigned int);
void           init8259(void);
int            kprintf(char * fmt, ...);
void           lidt(void);
void           outb(unsigned int, unsigned char);


#define MAX_PROC        64
#define MAX_OPEN_DEVS   4
#define NUM_OF_DEVICES   2
#define KERNEL_INT      80
#define TIMER_INT       (TIMER_IRQ + 32)
#define PROC_STACK      (4096 * 4)

#define STATE_STOPPED   0
#define STATE_READY     1
#define STATE_SLEEP     2

#define SYS_STOP        0
#define SYS_YIELD       1
#define SYS_CREATE      2
#define SYS_TIMER       3
#define SYS_GETPID      100
#define SYS_PUTS        103
#define SYS_SLEEP       105
#define SYS_SIGHANDLER  200
#define SYS_SIGRETURN   201
#define SYS_KILL        202

#define OPEN            205
#define CLOSE           206
#define WRITE           207
#define READ            208
#define IOCTL           209

typedef void    (*funcptr)(void);

typedef struct device {
    int majorNumber;
    int (*open)(int);
    int (*close)(int);
    int (*write)(int, void*, int);
    int (*read)(int, void*, int);
    int (*ioctl)(unsigned long, unsigned int);
} device;

typedef struct struct_pcb pcb;
struct struct_pcb {
    pcb         *next;
    pcb         *prev;
    long        esp;
    int         state;
    int         pid;
    int         otherpid;
    void        *buffer;
    int         bufferlen;
    int         ret;
    int         sleepdiff;
    long        args;
    unsigned int mask;
    void*       *signalblock[32];
    struct context_frame     *ctx_frm; // Value set in create.c and used in sigtramp
    struct device        FDT[MAX_OPEN_DEVS];
};


/* The actual space is set aside in create.c */
extern pcb     proctab[MAX_PROC];

#pragma pack(1)

typedef struct context_frame {
  unsigned int        edi;
  unsigned int        esi;
  unsigned int        ebp;
  unsigned int        esp;
  unsigned int        ebx;
  unsigned int        edx;
  unsigned int        ecx;
  unsigned int        eax;
  unsigned int        iret_eip;
  unsigned int        iret_cs;
  unsigned int        eflags;
  unsigned int        stackSlots[0];
} context_frame;



void     dev_init(void);
void     kfree(void *ptr);
void     kmeminit( void );
void     *kmalloc( int size );
void     dispatch( void );
void     dispatchinit( void );
void     ready( pcb *p );
pcb      *next( void );
void     contextinit( void );
int      contextswitch( pcb *p );
int      create( funcptr fp, int stack );
void     printCF (void * stack);
void     root( void );
void     set_evec(unsigned int xnum, unsigned long handler);
void     sleep(pcb *, int);
int      syscall(int call, ...);
int      syscreate( funcptr fp, int stack );
int      sysgetpid( void );
unsigned int syssleep( unsigned int t );
int      sysstop( void );
void     sysputs( char *str );
int      sysyield( void );
void     syssigreturn(void *old_sp);
int      syssighandler(int signal, void (*newhandler)(void *), void (** oldHandler)(void *));
int      syskill(int pid, int signalNumber);
void     tick( void );
int     open_kbd(int);
int     close_kbd(int);
int     write_kbd(int, void*, int);
int     read_kbd(int, void*, int);
int     ioctl_kbd(unsigned long, unsigned int);

void sigtramp (void (*handler)(void *), void *);
int signal(int, int);
int sigHandler(int sigNum, void (*new_handler)(void *), void (** old_handler)(void *), pcb *p);

void sigtrampFrame(pcb *p, int sig_no);
void checksignal(pcb *p);

extern int sysopen(int device_no);
extern int sysclose(int fd);
extern int syswrite(int fd, void *buff, int bufflen);
extern int sysread(int fd, void *buff, int bufflen);
extern int sysioctl(int fd, unsigned long command, ...);

int di_open(int device_no, pcb *p);
int di_close(int device_no, pcb *p);
int di_write(int fd, void *buff, int bufflen, pcb *p);
int di_read(int readfd, void *buff_read, int bufflen_read, pcb *p);
int di_ioctl(int fd, unsigned long command, unsigned int args, pcb *p);
