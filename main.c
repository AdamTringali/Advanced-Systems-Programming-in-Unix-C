#include <stdio.h>
#include "header.h"
#include <stdlib.h>
#include <string.h>

char* list[] = {
    "something",
    "else",
    "two"
};

int main(int argc, char** argv)
{  
    int flags = 0x0;


    int mallocVal = 0;
    void* buf = NULL;
    mallocVal = lkmalloc(10, &buf, flags);

    printf("Mallocval: %d\n", mallocVal);
    if(mallocVal < 0){
            printf("MallocVal error.");
            exit(1);
    }
    printf("%d:%p:",10, &buf);
    unsigned char *ptr = buf;
    for (int i = 0; i < 18; i++) {
        printf("%02x ", (int)ptr[i]);
    }
    printf("\n");

    int freeVal = 0;
    freeVal = lkfree(&buf, flags);
    printf("Freeval: %d\n", freeVal);


    printf("end of main\n");

    
    //on_exit((void*)lkreport,(void*)list);

    return 0;

}