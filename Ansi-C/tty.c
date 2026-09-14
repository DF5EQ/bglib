/*
    based on https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include "tty.h"

// Create new termios struct, we call it 'tty' for convention
static struct termios tty_old;
static struct termios tty_new;
static int serial_port;

int tty_open(void) {
  // Open the serial port. Change device path as needed (currently set to a standard FTDI USB-UART cable type device)
  serial_port = open("/dev/ttyACM0", O_RDWR);

  // Check for errors
  if (serial_port < 0) {
      printf("Error %i from open: %s\n", errno, strerror(errno));
      return 1;
  }

  // Read in existing settings, and handle any error
  if (tcgetattr(serial_port, &tty_old) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      tty_close();
      return 1;
  }

  memcpy(&tty_new, &tty_old, sizeof(struct termios));

  tty_new.c_cflag &= ~PARENB;        // Clear parity bit, disabling parity (most common)
  tty_new.c_cflag &= ~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
  tty_new.c_cflag &= ~CSIZE;         // Clear all bits that set the data size
  tty_new.c_cflag |= CS8;            // 8 bits per byte (most common)
  tty_new.c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control (most common)
  tty_new.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty_new.c_lflag &= ~ICANON;
  tty_new.c_lflag &= ~ECHO;                   // Disable echo
  tty_new.c_lflag &= ~ECHOE;                  // Disable erasure
  tty_new.c_lflag &= ~ECHONL;                 // Disable new-line echo
  tty_new.c_lflag &= ~ISIG;                   // Disable interpretation of INTR, QUIT and SUSP
  tty_new.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty_new.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty_new.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty_new.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

  tty_new.c_cc[VTIME] = 0;
  tty_new.c_cc[VMIN] = 1;
  
  cfsetispeed(&tty_new, B9600); // Set in baud rate to be 9600
  cfsetospeed(&tty_new, B9600); // Set out baud rate to be 9600

  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty_new) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      close(serial_port);
      return 1;
  }
}

void tty_write(char ch) {
  write(serial_port, &ch, 1);
}

int tty_read(void) {
  char ch;
  read(serial_port, &ch, 1);
  return ch;
}

void tty_close(void) { 
  close(serial_port);
  tcsetattr(serial_port, TCSANOW, &tty_old);
}
