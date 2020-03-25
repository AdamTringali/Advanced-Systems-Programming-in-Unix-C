#include <stdio.h>
#include "header.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char** reports;
int len_reports;

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

    mallocVal = lkmalloc(10, &buf, flags);
    if(mallocVal < 0){
        printf("MallocVal error. Exiting.\n");
        exit(1);
    }

//     mallocVal = lkmalloc(20, &buf2, flags);
//     if(mallocVal < 0){
//         printf("MallocVal error (2). Exiting.\n");
//         exit(1);
//     }
//    // buf2 = buf2 + 1;
//     freeVal = lkfree(&buf2, 0x2);
//     if(freeVal != 0)
//     {
//         if(errno == EINVAL)
//         {
//             printf("LKF_REF: ptr wassed was not exactly as allocated (1).\n");
//         }
//         else
//         {
//             fprintf(stderr,"free error (2). Exiting\n");
//             exit(1);
//         }
//     }
    buf = buf + 1;
    freeVal = lkfree(&buf, 0x2);
    if(freeVal != 0)
    {
        if(errno == EINVAL)
        {
            printf("LKF_REF: ptr wassed was not exactly as allocated (2).\n");
        }
        else
        {
            fprintf(stderr,"free error (2). Exiting\n");
            exit(1);
        }
    }

    
    on_exit((void*)lkreport,(void*)reports);

    return 0;

}