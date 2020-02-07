#include "functions.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int readFile(int fd, void* buf , int len)
{
    int rv = -1;
    while(rv = read(fd, buf, len) != 0 | rv <! 0){
    if (rv < 0) { // failed
      return 1;
    }
  }
  return 0;

}

int writeFile(int fd, void* buf, int len)
{    
    return write(fd, buf, strlen(buf));

}