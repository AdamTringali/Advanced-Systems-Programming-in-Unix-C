#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"


int lkmalloc(int size, void **ptr, int flags)
{

    on_exit((void*)lkreport,0);
    // int header = 8;
    // size += header;

    printf("lkmalloc.c flags: %x\n", flags);
    if(flags & LKM_UNDER | flags & LKM_OVER){
        size += 8;
    }
    
    *ptr = malloc(size);
    if(*ptr == NULL)
        return -1;
    char text[255];
    sprintf(text, "%p\n",*ptr);
    printf("test:%s\n",text);
    //STORING MEM ADDY IN HEADER
    // size_t int_value = (size_t)*ptr;
    // printf("here1\n");
    // memset(&ptr, int_value, 8);
    // *ptr = *ptr + 8;

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

    char text2[255];
    sprintf(text2, "%p\n",*ptr);
    printf("test2:%s\n",text2);

    // RETURN ALLOC SIZE (includes upper/lower 8 bytes)
    return size;
}


int lkfree(void **ptr, int flags)
{
    if(ptr == NULL)
        return 99;

    if(flags & LKM_UNDER){
        printf("underflag\n");
        *ptr = *ptr - 8;
    }

    free(*ptr);

    return 0;
}

int lkreport(int fd, void* flags)
{
    printf("\n\n -------lkreport------- \n");
    //dprintf - prints to file descriptor instead of i/o stream
    
    // for(int i = 0; i < 3; i++){
    //     printf("%d,%s\n", fd, ((char**)flags)[i]);

    // }

    //record_type,filename,fxname,line_num,timestamp,ptr_passed,retval,size_or_flags,
    //alloc_addr_returned

}