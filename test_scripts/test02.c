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
    
    int freeVal, mallocVal;
    void* buf = NULL;

    create_first_csv();

    on_exit((void*)lkreport,(void*)0x8);


    log_info(file_name, fxn_name, line_num);
    freeVal = lkfree(&buf, 0x0);


    exit(STDIN_FILENO);

    return 0;

}