
#include "wrapper.h"

// error function to handle system call errors
void sys_error(char *syscall)
{
    fprintf(stderr,"%s : %s\n",syscall,strerror(errno));
    exit(EXIT_FAILURE);
}

// wrapper for calloc
void* CALLOC(int num,int size)
{
    void *ptr = calloc(num,size);

    if(ptr == NULL)
    {
        fprintf(stderr, "CALLOC Failure. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

// wrapper for fork
pid_t FORK()
{
    pid_t pid = fork();
    if(pid == -1)
        sys_error("fork() error");
    return pid;
}

// wrapper for write, handling partial writes
size_t WRITE(int fd,void *buf,size_t count)
{
    ssize_t numwritten = 0;
    size_t numleft = count;
    char *bufptr = buf;
    
    while(numleft > 0)
    {
        if((numwritten = write(fd,buf,numleft)) <= 0)
        {
            if(errno == EINTR)
            {
                numwritten = 0;
            }
        
            else
                sys_error("write() error");
        }
        numleft = numleft - numwritten;
        bufptr = bufptr + numwritten;
    }
    return count;
}

// wrapper for read, handling partial reads
size_t READ(int fd,void *buf,size_t count)
{
    ssize_t numread = 0;
    size_t numleft = count;
    char *bufptr = buf;
    
    while(numleft > 0)
    {
        if((numread = read(fd,buf,numleft)) < 0)
        {
            if(errno == EINTR)
                numread = 0;
            else if(errno == EAGAIN)
            {
                if(numleft == count)
                    return 0;
                else
                {
                    numread = 0;
                }
                
            }
            else
                sys_error("read() error");
        }
        else if(numread == 0)
        {
            return -1;
            break;
        }

        numleft = numleft - numread;
        bufptr = bufptr + numread;
    }
    return count-numleft;
}

// wrapper for dup
int DUP(int oldfd)
{
    int fd;
    if((fd = dup(oldfd)) < 0)
        sys_error("dup error");
    return fd;
}
// wrapper for dup2
void DUP2(int oldfd,int newfd)
{
    if(dup2(oldfd,newfd) < 0)
        sys_error("dup2() error");
}

// wrapper for nice
void NICE(int priority)
{
    errno = 0;
    int ret = nice(priority);
    if(ret == -1 && errno != 0)
        sys_error("nice() error");
    
}

// wrapper for open
int OPEN(const char *pathname, int flags, mode_t mode)
{
    int fd;

    if ((fd = open(pathname, flags, mode)) < 0)
        sys_error("open() error");
    return fd;
}