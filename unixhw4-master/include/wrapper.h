#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

// debug macro
#ifndef _MYDEBUG_H
#define _MYDEBUG_H
#ifdef MYDEBUG
#define debug(...) printf( __VA_ARGS__)
#else
#define debug(...)
#endif
#endif

// wrapper function for system calls or libc functions
void sys_error(char *syscall);
void* CALLOC(int num,int size);
size_t WRITE(int fd,void *buf,size_t count);
size_t READ(int fd,void *buf,size_t count);
pid_t FORK();
double current_time();
void DUP2(int oldfd,int newfd);
int DUP(int oldfd);
void NICE(int priority);
int OPEN(const char *pathname, int flags, mode_t mode);
