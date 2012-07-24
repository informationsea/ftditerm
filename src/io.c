#include "io.h"

#ifndef __WINDOWS__
static struct termios oldtermios;
#endif
static int inited = 0;

void console_close()
{
  if (inited) {
#ifndef __WINDOWS__
    tcsetattr(STDOUT_FILENO,TCSANOW, &oldtermios);
#endif
  }
}

void console_init()
{

#ifndef __WINDOWS__
  struct termios newtermios;

  fflush(stdout);

  tcgetattr(STDOUT_FILENO,&oldtermios);
  newtermios = oldtermios;
  cfmakeraw(&newtermios);
  tcsetattr(STDOUT_FILENO,TCSANOW, &newtermios);
#endif
  inited = 1;
  atexit(console_close);
}

void cput(const char *str)
{
#ifndef __WINDOWS__
  write(STDOUT_FILENO,str, strlen(str));
#else
  _cputs(str);
#endif
}

void cputn(const char *str, int len)
{
#ifndef __WINDOWS__
  write(STDOUT_FILENO,str, len);
#else
  char buf[128];
  memset(buf,0,sizeof(buf));
  strncpy(buf,str,len);
  buf[len] = 0;
  _cputs(buf);
#endif
}

int cget(char *buf, int len)
{
#ifndef __WINDOWS__
  return read(STDIN_FILENO,buf, len);
#else
  int i;
  for(i = 0;i < len && _kbhit();i++) {
    *buf++ = _getch();
  }
  return i;
#endif
}

bool iskeydown(void)
{

#ifndef __WINDOWS__
  struct timeval ti;
  struct fd_set fdset;

  ti.tv_sec = 0;
  ti.tv_usec = 0;

  FD_ZERO(&fdset);
  FD_SET(STDIN_FILENO,&fdset);
  select(STDIN_FILENO+1, &fdset, NULL,NULL,&ti);
  if(FD_ISSET(STDIN_FILENO,&fdset)) {
    return true;
  } else {
    return false;
  }
#else
  return _kbhit();
#endif
}
