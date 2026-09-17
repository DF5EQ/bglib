#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "serial.h"

static struct termios serial_old;
static struct termios serial_new;
static int serial_port;

int serial_open(char* dev) {
    serial_port = open(dev, O_RDWR | O_NOCTTY);
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return -1;
    }

    /* save current settings of serial port */
    if (tcgetattr(serial_port, &serial_old) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        serial_close();
        return -1;
    }

    memcpy(&serial_new, &serial_old, sizeof(struct termios));

    serial_new.c_cflag &= ~PARENB;        // clear parity bit, disabling parity (most common)
    serial_new.c_cflag &= ~CSTOPB;        // clear stop field, only one stop bit used in communication (most common)
    serial_new.c_cflag &= ~CSIZE;         // clear all bits that set the data size
    serial_new.c_cflag |= CS8;            // 8 bits per byte (most common)
    serial_new.c_cflag &= ~CRTSCTS;       // cisable RTS/CTS hardware flow control (most common)
    serial_new.c_cflag |= CREAD | CLOCAL; // turn on READ & ignore ctrl lines (CLOCAL = 1)

    serial_new.c_lflag &= ~ICANON;
    serial_new.c_lflag &= ~ECHO;                   // disable echo
    serial_new.c_lflag &= ~ECHOE;                  // disable erasure
    serial_new.c_lflag &= ~ECHONL;                 // disable new-line echo
    serial_new.c_lflag &= ~ISIG;                   // disable interpretation of INTR, QUIT and SUSP
    serial_new.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off software based flow control (XON/XOFF)
    serial_new.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    serial_new.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    serial_new.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    serial_new.c_cc[VTIME] = 1; // read at least 1 byte
    serial_new.c_cc[VMIN]  = 0; // wait for 0*100ms
  
    cfsetispeed(&serial_new, B9600); // Set in baud rate to be 9600
    cfsetospeed(&serial_new, B9600); // Set out baud rate to be 9600

    /* store new settings */
    if (tcsetattr(serial_port, TCSANOW, &serial_new) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        close(serial_port);
        return -1;
    }

    /* Flush input and output buffers to clear out garbage */
    if (tcflush(serial_port, TCIOFLUSH) != 0) {
        printf("Error %i from tcflush: %s\n", errno, strerror(errno));
    }

    return 0;
}

void serial_write(char ch) {
    write(serial_port, &ch, 1);
}

uint16_t serial_read(void) {
    uint16_t ch;
    if (read(serial_port, &ch, 1) == 0) {
        return 256;
    } else {
        return ch;
    }
}

void serial_close(void) {
    /* restore old settings */
    tcsetattr(serial_port, TCSANOW, &serial_old);

    /* close serial port */
    close(serial_port);
}

