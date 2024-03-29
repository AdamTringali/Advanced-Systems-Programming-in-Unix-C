

//TESTS MALLOC WITH LKM_UNDER WITH LKR_MATCHES FLAG
#include <stdio.h>
#include <unistd.h>
#include "../header.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern char* file_name;
extern int line_num;
extern const char* fxn_name;

int main(int argc, char** argv)
{  
    
    int mallocVal;
    void* buf = NULL;

    create_first_csv();

    on_exit((void*)lkreport,(void*)0x2);

    log_info(file_name, fxn_name, line_num);
    mallocVal = lkmalloc(20, &buf, 0x4);
    if(mallocVal < 0){
        printf("MallocVal error. Exiting.\n");
        exit(1);
    }

    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x1);

    exit(STDIN_FILENO);

    return 0;

}