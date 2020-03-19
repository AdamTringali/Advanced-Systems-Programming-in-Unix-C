#include <stdio.h>
#include "header.h"
#include <stdlib.h>

int lkfree(void **ptr, int flags)
{
    if(ptr == NULL)
        return 99;

    if(flags & LKM_UNDER){
        printf("underflag\n");
        *ptr -= 8;
    }

    free(*ptr);

    return 5;
}