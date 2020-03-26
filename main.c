#include <stdio.h>
#include <unistd.h>
#include "header.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char** reports;
int len_reports;
char* file_name;
int line_num;
const char* fxn_name;

int main(int argc, char** argv)
{  
    reports = malloc(10 * sizeof(char*));
    len_reports = 0;
    int freeVal, mallocVal, flags = 0x0;
    void* buf = NULL;
    void* buf2 = NULL;

    // ADDING CSV FORMAT TO LKREPORT LIST
    int strlen = 500;
    reports[len_reports] = malloc(strlen * sizeof(char));
    strcpy(reports[len_reports++],"record_type,filename,fxname,line_nub,timestamp,ptr_passed,retval,size_or_flags,alloc_addr_returned");

    //lkfoo("main.c","")
    log_info(file_name, fxn_name, line_num);
    mallocVal = lkmalloc(10, &buf, flags);
    if(mallocVal < 0){
        printf("MallocVal error. Exiting.\n");
        exit(1);
    }

    log_info(file_name, fxn_name, line_num);
    mallocVal = lkmalloc(20, &buf2, flags);
    if(mallocVal < 0){
        printf("MallocVal error (2). Exiting.\n");
        exit(1);
    }

    buf2 = buf2 + 1;
    //buf2 = malloc(20);
    log_info(file_name, fxn_name, line_num);
    freeVal = lkfree(&buf2, 0x1);
    if(freeVal != 0)
    {
        if(errno == EINVAL)
        {
            fprintf(stderr,"EINVAL (1).\n");
        }
        else if(freeVal == 56)
        {
            fprintf(stderr,"Trying to free something in the middle of an allocated memory chunk.\nTo fix memory leak enable LKF_APPROX flag.\n");
        }
        else if(freeVal == 36)
        {
            //fprintf(stderr, )
        }
        else
        {
            fprintf(stderr,"freeval: %d\n", freeVal);
            fprintf(stderr,"Unhandled free error (2). Exiting\n");
            exit(1);
        }
    }

    //buf = buf + 1;
    log_info(file_name, fxn_name, line_num);
    freeVal = lkfree(&buf, 0x4);
    if(freeVal != 0)
    {
        if(errno == EINVAL)
        {
            fprintf(stderr,"EINVAL (2).\n");
        }
        else if(freeVal == 56)
        {
            fprintf(stderr,"Trying to free something in the middle of an allocated memory chunk.\nTo fix memory leak enable LKF_APPROX flag.\n");
        }
        else if(freeVal == 36)
        {
            //fprintf(stderr, )
        }
        else
        {
            fprintf(stderr,"freeval: %d\n", freeVal);
            fprintf(stderr,"Unhandled free error (3). Exiting\n");
            exit(1);
        }
    }
    //exit(0);
    on_exit((void*)lkreport,(void*)0x1);
    exit(STDIN_FILENO);

    return 0;

}