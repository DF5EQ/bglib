#ifndef TTY_H
#define TTY_h

int  tty_open(void);
void tty_write(char ch);
int  tty_read(void);
void tty_close(void);

#endif //TTY_H
