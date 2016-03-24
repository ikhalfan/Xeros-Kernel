/* di_calls.c : messaging system (assignment 2)
 */

#include <sys/ioctl.h>
#include "../h/xeroskernel.h"
#include "../h/kbd.h"

/* di_open checks to see if the device number is valid and if there is a device open already. If neither of these cases are true
 * then it returns. Else it calls the lower level code that opens the device.
 *
 * @param : device_number (Major Number) and the process
 *
 * return : -1 on fail. 0 on sucess.
 *
 * */
int di_open(int device_no, pcb *p) {
    kprintf("TEMPENTER0");
    kprintf("#di_calls:di_open: device number = %d", device_no);


    if (device_no < 0 || device_no >= NUM_OF_DEVICES) {
        return -1;
    }

    int i;
    int n;
    int empty_spot = -808;
    for (i = 0; i < MAX_OPEN_DEVS; i++) {
        n = p->FDT[i].majorNumber;
        if (n == -2) {
            empty_spot = i;
            kprintf("\n emptyspot = %d, n = %d ", empty_spot, n);

        } else if (n == device_no) {
            kprintf("\nsecond return");
            return -1;
        }
    }

    kprintf("\n before last if %d %d %d", empty_spot, n, i);
    if (empty_spot >= 0) {
        kprintf("TEMPENTER1");
        p->FDT[empty_spot].majorNumber = device_no;
        return p->FDT[empty_spot].open(device_no);
    }

    kprintf("SHOULD NOT BE HERE 4673");
    return -1;
}

/* di_close checks to see if the device number is valid and if there is a device open already with that number.
 * If there is one it calls the lower level close, else returns.
 *
 * @param : device_number (Major Number) and the process
 *
 * return : -1 on fail. 0 on success.
 *
 * */
int di_close(int device_no, pcb *p) {
    kprintf("#di_calls:di_close");

    if (device_no < 0 || device_no >= NUM_OF_DEVICES) {
        kprintf("printing -1");
        return -1;
    }

    int i;
    for (i = 0; i < MAX_OPEN_DEVS; i++) {
        int n = p->FDT[i].majorNumber;
        kprintf("#di_calls:major number the %d th iteration is %d", n, i);

        if (n == device_no) {
            kprintf("#di_calls:In the if statement");
            int k = p->FDT[i].close(device_no);
            p->FDT[i].majorNumber = -2;
            return k;
        }
    }
    kprintf("#di_calls:about to return -1");
    return -1;
}

/* di_write checks to see if the device number is valid and if there is a device open already. If the device is open
 * then it can write to it.
 *
 * @param : fd (Major Number), the buffer with the data, the length of the buffer, and the process
 *
 * return : -1 on fail, number of bytes written on success with a max of bufflen
 *
 * */
int di_write(int fd, void *buff, int bufflen, pcb *p) {

    if (fd < 0 || fd >= NUM_OF_DEVICES) {
        return -1;
    }

    int i;
    int j = 0;
    for (i = 0; i < MAX_OPEN_DEVS; i++) {
        if (p->FDT[i].majorNumber == fd) {
            j = i;
        }
    }
    if (j == 0) {
        return -1;
    }
    else return p->FDT[fd].write(fd, buff, bufflen);
}

/* di_read checks to see if the device number is valid and if there is a device open already. If the device is open
 * then it can read from it.
 *
 * @param : fd (Major Number), the buffer with the data, the length of the buffer, and the process
 *
 * return : -1 on fail, number of bytes read on success with a max of bufflen
 *
 * */
int di_read(int fd, void *buff, int bufflen, pcb *p)
{
    if (fd < 0 || fd >= NUM_OF_DEVICES) {
        return -1;
    }
    int i, j = 0;
    for (i = 0; i < MAX_OPEN_DEVS; i++) {
        if (p->FDT[i].majorNumber == fd) {
            j = i;
        }
    }
    if (j == 0) {
        return -1;
    }
    else return p->FDT[fd].read(fd, buff, bufflen);
}

/* di_ioctl allows the designer to change the certain parameters assigned such as the "stop read".
 *
 * @param : fd (Major Number), the assigned index of the code to run, extra arguments (if any)
 * such as the new "stop read"
 *
 * return : -1 on fail, 0 otherwise
 *
 * */
int di_ioctl(int fd, unsigned long command, unsigned int args, pcb *p)
{
    if (fd < 0 || fd >= NUM_OF_DEVICES) {
        return -1;
    }

    if (p->FDT[fd].majorNumber == -1) {
        return -1;
    }

    return p->FDT[fd].ioctl(command, args);
}