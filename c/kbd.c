/* kbd.c : messaging system (assignment 2)
 */

//#include <xeroskernel.h>
//#include <kbd.h>
#include <stdarg.h>
#include <stdbool.h>
#include "../h/xeroskernel.h"
#include "../h/kbd.h"
#include "../h/i386.h"


/**
 * kbd.c: The lower level code for the keyboard
 **/


unsigned char kdb_buff[4];
bool echo = 0;

void enable_irq(unsigned int, int);

/*
 * Initializes the buffer of size 4
 */
void kbd_init(void) {

    int i;
    for(i=0; i < (sizeof(kdb_buff)/ sizeof(kdb_buff[0])); i++){
        kdb_buff[i] = 0;
    }

}


/*
 * The open command. Differentiates between echo and non-echo
 * Calls the initialization for keyboard buffer
 * Enables the interrups for the keyboard
 * @param: n = major number
 * @return: 0. Will always return
 */
int open_kbd(int n) {

    if (n == 0) {
        echo = 0;
    } else if (n ==1) {
        echo == 1;
    }

    kbd_init();
    enable_irq(1, 0);
    return 0;
}

/*
 * Disables interrupt from the keyboard
 */
int close_kbd(int n) {
    enable_irq(1,1);
    return 0;
}

/*
 * Keyboards don't write and should not be called
 */
int write_kbd(int fd, void* buff, int bufflen) {
    kprintf("THIS SHOULD NOT BE PRINTED. KEYBOARDS DON'T WRITE");
    return -1;
}

/*
 * Reads the buffered values from the keyboard.
 * @param: fd = file descriptor
 * @param: buff = the buffer we're copying kbd_buff to
 * @param: bufflen = how many we want to put into the buffer
 * @return: the actual number of bytes read
 */
int read_kbd(int fd, void* buff, int bufflen) {
//    void* buffer[bufflen];
//    buffer = buff;
    int readSoFar = 0;
    int i, j = 0;

    for (i = 0; i < bufflen; i++) {
        //TODO: change this to buffer
        if (i == 0 && j < 4) {
            kdb_buff[j];
            readSoFar++;
            j++;
            if (i == bufflen-1) {
                return bufflen;
            }
        }
        if (j == 4) {
            return i+1;
        }

    }

    if (echo) {
        // TODO : add echo code here so that it is printer out on the monitor
    } else {
        // TODO : don't echo on the terminal, just pass it in the buffer
    }

    //TODO: edit this return statement
    return 5;
}

/*
 * Allows use to define what to do when control key is pressed
 * @pram: command = what you give as the new stop-read character
 * @param: args = extra arguments
 * @return: returns 0 on success or 01 on fail
 */
int ioctl_kbd(unsigned long command, unsigned int args) {
    char eof;

    va_list arg = (va_list) args;

    switch (command) {
        case 53:

            eof = (unsigned char) va_arg(arg, unsigned int);
            return 0;
        default:
            return -1;
    }
}
