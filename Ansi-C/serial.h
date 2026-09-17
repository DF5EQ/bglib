#ifndef SERIAL_H
#define SERIAL_h

#include <stdint.h>

int      serial_open(char* dev);
void     serial_write(char ch);
uint16_t serial_read(void);
void     serial_close(void);

#endif //SERIAL_H
