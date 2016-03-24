/* user.c : User processes
 */

//#include <xeroskernel.h>
//#include <xeroslib.h>
#include "../h/xeroslib.h"
#include "../h/xeroskernel.h"

//int killerPID, victimPID;

//void producer( void ) {
///****************************/
//
//    int         i;
//    char        buff[100];
//
//    sysopen(1);
//    sysclose(1);
//    sysclose(0);
//
//    for( i = 0; i < 20; i++ ) {
//        sysputs(buff);
//
//    }
//    for (i = 0; i < 15; i++) {
//        sysputs("P");
//        syssleep(1500);
//    }
//    sysputs( buff );
//    sysstop();
//}
//
//void consumer( void ) {
///****************************/
//
//    int         i;
//    char        buff[100];
//
//    for( i = 0; i < 10; i++ ) {
//        sysputs( buff );
//        sysyield();
//    }
//
//    for (i = 0; i < 22; i++) {
//        sysputs("C");
//        syssleep(1000);
//    }
//
//    sysputs( buff );
//    sysstop();
//}

void step_12(void) {
    syssleep(1000);
    //assuming 1 is root
    signal(1,20);
    signal(1,18);
    return;
}

void step_16(void) {
    syssleep(5000);
    //assuming 1 is root
    signal(1,18);
    return;
}

void $step_20(void) {
    syssleep(5000);
    //assuming 1 is root
    signal(1,20);
    return;
}

void i_was_called(void) {
    kprintf("I was called!\n");
}

void i_was_called_again(void) {
    kprintf("I'm a different signal!\n");
}


void     root( void ) {
    kprintf("Greeting!");

    //open the echo keyboard
    sysopen(1);

    //read 10 characters
    char keyboard_characters[10];
    sysread(1, keyboard_characters, 10);
    sprintf(keyboard_characters, "Characters\n");

    //attempt to open "echo" and "no echo"
    sysopen(0);
    sysopen(1);

    //closes the keyboard
    sysclose(0);
    sysclose(1);

    //open the echo keyboard
    sysopen(0);

    //read three times
    char chr[10];
    int i = 0;
    while (i < 3) {
        sysread(0, chr, 10);
        sprintf(chr, "chr\n");
        i++;
    }

    //continue reading until eof
    int j = 0;
    while (j == 0) {
        j = sysread(0, chr, 10);
    }

    //closes the keyboard and opens echo
    sysclose(0);
    sysopen(1);

    //Installs a signal handler for signal 18 that
    //simply prints that it was called.
    syssighandler(18, (void (*)(void *)) i_was_called, NULL);

    //Creates a new process that sleeps for 1 second and then
    //sends a signal 20 to the root process followed by signal
    //18 and then exits.
    syscreate(&step_12, 4096);

    //14: expect an error
    int expected_error;
    expected_error = sysread(1, chr, 10);
    kprintf("Error is: %d\n", expected_error);

    //15: Installs a signal handler for signal 18 that simply
    //prints that it is the 2nd signal handler and was called.
    //The handler that was being replaced, and is returned via
    //the 3rd argument to syssighandler(), is remembered.

    int returned_address = 0;
    returned_address = syssighandler(18, (void (*)(void *)) i_was_called_again, (void (**)(void *)) i_was_called);

    //16. Creates a new process that sleeps for 5 seconds and then
    //sends a signal 18 to the root process and then exits
    syscreate(&step_16, 4096);

    //17 and 18: expect an error
    int expected_error_interrupt;
    expected_error_interrupt = sysread(1, chr, 10);
    kprintf("Error is: %d\n", expected_error_interrupt);

    //19. Installs a signal handler for signal 20. The handler
    // that is installed is the address returned from the previous
    // call to syssighandler().
    syssighandler(20, (void (*)(void *)) returned_address, (void (**)(void *)) i_was_called_again);

    //20. Creates a new process that sleeps for 5 seconds and
    // then sends a signal 20 to the root process and then exits.
    syscreate($step_20, 4096);

    //21 and 22: expect an error again
    int expected_error_interrupt_again;
    expected_error_interrupt_again = sysread(1, chr, 10);
    kprintf("Error is: %d\n", expected_error_interrupt_again);

    //23: continue reading until eof
    int k = 0;
    while (k == 0) {
        k = sysread(0, chr, 10);
    }

    //24: attempt to read again
    int attempt_to_read;
    attempt_to_read = sysread(0, chr, 10);
    kprintf("atempt to read something %d\n", attempt_to_read);

    kprintf("It's finallt over!!!!!!");
    sysstop();



//    char buff[100];
//    sysputs("Root has been called\n");
//
//    sysyield();
//    sysyield();
//
//    syscreate(&producer, 4096);
//    syscreate(&consumer, 4096);
//
//    sprintf(buff, "Root finished\n");
//    sysputs(buff);
//    sysstop();
}

