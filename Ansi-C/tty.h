#ifndef TTY_H
#define TTY_h

#include <stdint.h>

int      tty_open(char* dev);
void     tty_write(char ch);
uint16_t tty_read(void);
void     tty_close(void);

#endif //TTY_H
