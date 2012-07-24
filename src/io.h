#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifndef __WINDOWS__
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#else
#include <conio.h>
#include <windows.h>
#endif

typedef char bool;
#define true 1
#define false 0

void console_close(void);
void sigint(int s);
void console_init(void);
void cput(const char *str);
void cputn(const char *str,int len);
int  cget(char *buf,int len);
bool iskeydown(void);

#endif /* IO_H_ */
