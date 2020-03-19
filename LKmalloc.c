#include <stdlib.h>
#include <stdio.h>
#include "header.h"


int lkmalloc(int size, void **ptr, int flags)
{
    printf("lkmalloc.c flags: %x\n", flags);
    if(flags & LKM_UNDER){
        printf("underflag\n");
        size += 8;
    }
    
    *ptr = malloc(size);
    if(*ptr == NULL)
        return -1;

    if(flags & LKM_UNDER)
        *ptr = *ptr + 8;


    // IF LKM_UNDER 0x4 
    // ret pointer + 8
    return size;
}
