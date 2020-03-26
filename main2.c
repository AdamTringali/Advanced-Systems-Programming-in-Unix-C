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
    reports = malloc(200 * sizeof(char*));
    len_reports = 0;
    int freeVal, mallocVal, flags = 0x0;
    void* buf = NULL;
    void* buf2 = NULL;

    // ADDING CSV FORMAT TO LKREPORT LIST
    int strlen = 500;
    reports[len_reports] = malloc(strlen * sizeof(char));
    strcpy(reports[len_reports++],"record_type,filename,fxname,line_nub,timestamp,ptr_passed,retval,size_or_flags,alloc_addr_returned");

    on_exit((void*)lkreport,(void*)0x1);

   // for(int i = 0; i < 2; i++)
    //{
        log_info(file_name, fxn_name, line_num);
        mallocVal = lkmalloc(10, &buf, flags);
        if(mallocVal < 0){
            printf("MallocVal error. Exiting.\n");
            exit(1);
        }

               log_info(file_name, fxn_name, line_num);
        mallocVal = lkmalloc(10, &buf2, flags);
        if(mallocVal < 0){
            printf("MallocVal error. Exiting.\n");
            exit(1);
        }

        log_info(file_name, fxn_name, line_num);
        //buf += 1;
        freeVal = lkfree(&buf, 0x1);
        if(freeVal != 0)
        {
            printf("non 0 return val: ret:%d\n", freeVal);
            exit(1);
        }
        printf("next round \n\n");
   


        // log_info(file_name, fxn_name, line_num);
        // //buf2 += 1;
        // freeVal = lkfree(&buf2, 0x1);
        // if(freeVal != 0)
        // {
        //     printf("non 0 return val: ret:%d\n", freeVal);
        //     exit(1);
        // }

        
    

  

    exit(STDIN_FILENO);

    return 0;

}