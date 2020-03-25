#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include <time.h>
#include <errno.h>

extern char** reports;
extern int len_reports;

int lkmalloc(int size, void **ptr, int flags)
{

    if(flags & LKM_UNDER | flags & LKM_OVER){
        size += 8;
    }

    *ptr = malloc(size);
    
    if(*ptr == NULL)
        return -1;

    // LKM_INIT - initalize memory being allocated to 0
    if(flags & LKM_INIT)
    {
        printf("init_flag\n");
        memset(&ptr, 0, size);
    }

    // LKM_OVER - alloc. 8 more bytes and write 0x5a in upper 8 
    if(flags & LKM_OVER)
    {
        printf("in lkm over\n");
        *ptr = *ptr + (size - 8);
        memset(*ptr, 0x5a, 8);
        *ptr = *ptr - (size-8);
    }

    // LKM_UNDER - alloc. 8 more bytes and write 0x6b in lower 8
    if(flags & LKM_UNDER)
    {
        printf("in lkm under\n");
        memset(*ptr, 0x6b, 8);
        *ptr = *ptr + 8;
    }


    // ADDING REPORT TO THE LIST OF REPORTS
    int strlen = 500;
    reports[len_reports] = malloc(strlen * sizeof(char));

    char address[255];
    snprintf(address,sizeof(address), "lkmalloc,filename,fxname,line_nub,timestamp,ptr_passed,%d,size_or_flags,%p",size, *ptr);
    memcpy(reports[len_reports++], address, sizeof(address));

    // RETURN ALLOC SIZE (includes upper/lower 8 bytes)
    return size;
}



int lkfree(void **ptr, int flags)
{
    
    int ret = 0;
    int strlen = 500;
    reports[len_reports] = malloc(strlen * sizeof(char));

    char address[255];
    snprintf(address, sizeof(address), "%p", *ptr);


    //LKF_REG - free only if the ptr passed was exactly as alloc
    if(!flags & !LKF_REG)
    { // free only if the ptr passed was exactly as was allocated
        int found = 0;
        for(int i = 1; i < len_reports; i++){
            char *t1 = strdup(reports[i]);
            char *item = strtok(t1, ",");
            if(strcmp("lkmalloc", item) == 0)
            {//MALLOC RECORD, CHECK ALLOC_ADDR_RETURNED 
                for(int i = 0; i < 8; i++)
                    item = strtok(NULL, ",");
                
                //NOW ITEM CONTAINS THE ALLOC_ADDR_RETURNED ADDRESS. COMPARE WITH PTR ADDRESS
                if(strcmp(address, item) == 0)
                    found = 1;
            }
            free(t1);
        }
        if(found == 0){
            //CAUSES MEMORY LEAK
            errno = EINVAL;
            return 1;
        }
    }

    //LKF_APPROX - free alloc even if what is passed is in the middle of a valid alloc.
    
    if(flags & LKF_APPROX)
    {
        printf("LKF_APPROX. ptr addy: %s\n", address);
        //*ptr = *ptr - 1;
        int found = 0;
        char *t1, *item, *potential_real, *potential_wrong;
        int size = 0;
        for(int i = 1; i < len_reports; i++){
            t1 = strdup(reports[i]);
            item = strtok(t1, ",");
            if(strcmp("lkmalloc", item) == 0)
            {//MALLOC RECORD, CHECK ALLOC_ADDR_RETURNED 
                for(int i = 0; i < 6; i++)
                    item = strtok(NULL, ",");
                size = atoi(item);
                for(int i = 0; i < 2; i++)
                    item = strtok(NULL, ",");
                //NOW ITEM CONTAINS THE ALLOC_ADDR_RETURNED ADDRESS. COMPARE WITH PTR ADDRESS

                int real_address;// = atoi(item);
                int ptr_address;
                sscanf(item,"%x", &real_address);
                sscanf(address,"%x", &ptr_address);
                if((ptr_address - real_address) != 0)
                    if(ptr_address > real_address){
                        if((ptr_address - real_address) <= size && real_address == (ptr_address - (ptr_address - real_address))  )
                        {
                            *ptr = *ptr - (ptr_address - real_address);
                            printf("new addy: %p\n", *ptr);
                        }
                    }
            }
            free(t1);
        }

    }

    if(ptr == NULL)
        return 99;

    if(flags & LKM_UNDER){
        printf("underflag\n");
        *ptr = *ptr - 8;
    }

    free(*ptr);
    snprintf(address,sizeof(address), "lkfree,filename,fxname,line_nub,timestamp,ptr_passed,retval,size_or_flags,(null)");
    memcpy(reports[len_reports++], address, sizeof(address));

    return 0;
}

int lkreport(int fd, void* flags)
{


    printf("\n                                -------lkreport------- \n");
    //dprintf - prints to file descriptor instead of i/o stream
    //printf("len_reports: %d\n", len_reports);

    for(int i = 0; i < len_reports; i++){
        printf("%s\n", reports[i]);
        free(reports[i]);
    }
    free(reports);
}