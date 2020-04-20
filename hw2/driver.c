#include <stdio.h>
#include <unistd.h>
#include "header.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>


extern char* file_name;
extern int line_num;
extern const char* fxn_name;

int main(int argc, char** argv)
{  
    void* buf = NULL;

    //on_exit((void*)lkreport,(void*)0xff);
    printf("Testing malloc & free with no flags, all lkreport flags\n");
    create_first_csv();
    log_info(file_name, fxn_name, line_num);
    lkmalloc(20, &buf, 0x0);
    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x0);
    lkreport(STDIN_FILENO, 0xff);

    printf("\nTesting malloc with LKM_INIT flag, free w/ no flags, all lkreport flags\n");
    create_first_csv();
    log_info(file_name, fxn_name, line_num);
    lkmalloc(20, &buf, 0x1);
    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x0);
    lkreport(STDIN_FILENO, 0xff);

    printf("\nTesting malloc with LKM_OVER flag, free w/ no flags, all lkreport flags\n");
    create_first_csv();
    log_info(file_name, fxn_name, line_num);
    lkmalloc(20, &buf, 0x2);
    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x0);
    lkreport(STDIN_FILENO, 0xff);

    printf("\nTesting malloc with LKM_UNDER flag, free w/ no flags, all lkreport flags\n");
    create_first_csv();
    log_info(file_name, fxn_name, line_num);
    lkmalloc(20, &buf, 0x4);
    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x0);
    lkreport(STDIN_FILENO, 0xff);
    
    printf("\nTesting free w/ LKF_REG flag (0x0) & invalid address, all lkreport flags\n");
    create_first_csv();
    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x0);
    lkreport(STDIN_FILENO, 0xff);

    printf("\nTesting free w/ LKF_APPROX flag & alloc'd address in middle of block, all lkreport flags\n");
    create_first_csv();
    log_info(file_name, fxn_name, line_num);
    lkmalloc(20, &buf, 0x0);
    buf++;
    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x1);
    lkreport(STDIN_FILENO, 0xff);

    printf("\nTesting free w/ no flags & alloc'd address in middle of block, all lkreport flags\n");
    create_first_csv();
    log_info(file_name, fxn_name, line_num);
    lkmalloc(20, &buf, 0x0);
    buf++;
    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x0);
    lkreport(STDIN_FILENO, 0xff);

    printf("\nTesting free w/ LKF_WARN & LKF_APPROX flags & alloc'd address in middle of block, all lkreport flags\n");
    create_first_csv();
    log_info(file_name, fxn_name, line_num);
    lkmalloc(20, &buf, 0x0);
    buf++;
    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x3);
    lkreport(STDIN_FILENO, 0xff);

    printf("\nTesting free w/ LKF_UNKNOWN flag & unallocated memory address, all lkreport flags\n");
    create_first_csv();
    log_info(file_name, fxn_name, line_num);
    lkfree(&buf, 0x4);
    lkreport(STDIN_FILENO, 0xff);

    // printf("\nTesting free w/ LKF_ERROR flag & unallocated memory address, all lkreport flags\n");
    // create_first_csv();
    // log_info(file_name, fxn_name, line_num);
    // lkfree(&buf, 0x8);
    // lkreport(STDIN_FILENO, 0xff);

    return 0;

}