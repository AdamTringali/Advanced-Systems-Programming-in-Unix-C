#include <stdio.h>
#include "header.h"
#include <stdlib.h>

int main(int argc, char** argv)
{  
    int flags = 0x4;


    int mallocVal = 0;
    void *buf = NULL;
    mallocVal = lkmalloc(10, &buf, flags);
    // buf = malloc(10);
    printf("Mallocval: %d\n", mallocVal);

    int freeVal = 0;
    freeVal = lkfree(&buf, flags);
    printf("Freeval: %d\n", freeVal);


    printf("end of main\n");
    return 0;


}