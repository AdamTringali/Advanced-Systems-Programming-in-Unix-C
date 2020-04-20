#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <execinfo.h>
#include <unistd.h>

// extern char** reports;
// extern int len_reports;
// extern char* file_name;
// extern char *fxn_name;
// extern int line_num;

char** reports;
int len_reports;
char* file_name;
char *fxn_name;
int line_num;
int under_malloc[200];
int und_malloc_count;
/*
*create_first_csv: creates the first line to the report for lkreport
* containg the field descriptions
*/
void create_first_csv()
{
    und_malloc_count = 0;
    //under_malloc[200];
    reports = malloc(200 * sizeof(char*));
    len_reports = 0;
    int strlen = 500;
    reports[len_reports] = malloc(strlen * sizeof(char));
    strcpy(reports[len_reports++],"record_type,filename,fxname,line_nub,timestamp,ptr_passed,retval,size_or_flags,alloc_addr_returned");
}
/*
*get_num_mallocs:
* returns the number of '0' records in the reports list
*/
int get_num_mallocs()
{
    int count = 0;
    char *t1, *item = NULL;
    for(int i = 0; i < len_reports; i++)
    {
        t1 = strdup(reports[i]);
        item = strtok(t1, ",");
        if(strcmp("0", item) == 0)
            count++;
        free(t1);
    }

    //printf("num_mallocs: %d\n", count);
    return count;
}
/*
*find_serious_leaks: takes in a array of ints to update
* returns the length of the updated list
*/
int find_serious_leaks(int list[])
{
    int count = 0;
    char *t1, *t2, *item = NULL;

    for(int i = 1; i < len_reports; i++)
    {
        char *addy;
        int difference;
        t2=strdup(reports[i]);
        item = strtok(t2, ",");
        if(strcmp("1", item) == 0 /* == 0 && strstr(reports[i], ",56,") != NULL */)
        {
            for(int k = 0; k < 4; k++)
                item = strtok(NULL, ",");
            addy = strtok(NULL, ",");
            item = strtok(NULL, ",");
            difference = atoi(item) - 100;
            for(int z = 0; z < len_reports; z++)
            {
                t1=strdup(reports[z]);
                item = strtok(t1, ",");
                if(strcmp("0", item) == 0)
                {
                    
                    for(int k = 0; k < 7; k++)
                        item = strtok(NULL, ",");
                    if(strtoul(addy,NULL,0) - difference == strtoul(item,NULL,0))
                    {
                        //printf("found the malloc.\n");
                        list[count++] = z;
                    }
                }
                free(t1);
            }            
        }
        free(t2);

    }
    return count;
}
/*
*find_approx_frees: takes in a array of ints to update
* returns the length of the updated list
*/
int find_approx_frees(int list[])
{
    int count = 0;
    char *t1, *t2,*addy,*size, *item = NULL;

    for(int i = 1; i < len_reports; i++)
    {
        t1=strdup(reports[i]);
        item = strtok(t1, ",");
        
        if(strcmp("1", item) == 0)
        {
            if(strstr(reports[i], ",50,") != NULL)
            {
                list[count++] = i;
            }
        }
        free(t1);
    }
    int x1 = count;

    for(int i = 0; i < x1; i++)
    {
        t1 = strdup(reports[list[i]]);
        item = strtok(t1,",");
        for(int k = 0; k < 5; k++)
            item = strtok(NULL, ",");
        //printf("searching for malloc report for: %s\n", item);
        
        for(int k = 0; k < len_reports; k++)
        {
            t2 = strdup(reports[k]);
            size = strtok(t2,",");
            if(strcmp(size,"0") == 0)
            {
                for(int z = 0; z < 6; z++)
                    size = strtok(NULL,",");
                addy = strtok(NULL,",");
                //size = addy;
                //printf("size:%d\n",atoi(size));
                //printf("addy:%ld\n",strtoul(addy, NULL, 0));
                //printf("item:%ld\n",strtoul(item, NULL, 0));
                unsigned long int a1 = strtoul(addy,NULL,0);
                unsigned long int a2 = strtoul(item,NULL,0);
                if(a1 + atoi(size) > a2)
                {
                    list[count++] = k;
                }
            }
            free(t2);

        }
        // if(strtoul(addy,NULL,0) - difference == strtoul(item,NULL,0))
        // {
        //     //printf("found the malloc.\n");
        //     list[count++] = z;
        // }

        free(t1);
    }
    return count;
}
/*
*find_bad_frees: takes in a array of ints to update
* returns the length of the updated list
*/
int find_bad_frees(int list[])
{
    int count = 0;
    char *t1, *item = NULL;

    for(int i = 1; i < len_reports; i++)
    {
        t1=strdup(reports[i]);
        item = strtok(t1, ",");
        
        if(strcmp("1", item) == 0)
        {
            for(int z = 0; z < 6; z++)
                item = strtok(NULL, ",");
            

            if(strstr(reports[i], ",50,") != NULL || atoi(item) > 100)
            {
                list[count++] = i;
            }
        }
        free(t1);
    }
    return count;
}
/*
*find_orphan_frees: takes in a array of ints to update
* returns the length of the updated list
*/
int find_orphan_frees(int list[])
{
    int count = 0;
    char *t1, *item = NULL;

    for(int i = 1; i < len_reports; i++)
    {
        t1=strdup(reports[i]);
        item = strtok(t1, ",");
        if(strcmp("1", item) == 0 && strstr(reports[i], ",22,") != NULL)
        {
            list[count++] = i;
        }
        free(t1);
    }
    return count;
}

/*
*find_matches: takes in a array of ints to update
* returns the length of the updated list
*/
int find_matches(int list[])
{
    int count = 0;
    char *t1, *item = NULL;
    char *memaddy = NULL;
    int num_m = get_num_mallocs();
    int index = 1;
    int j;
    int new = 1;
    char **newlist = malloc(sizeof(char*) * len_reports);
    memcpy(newlist, reports, sizeof(char*) * len_reports);
    
    for(int i = 0; i < num_m; i++)
    {
        for(j = new; j < len_reports; j++)
        {
            //printf("j:%d\n", j);
            t1 = strdup(newlist[j]);
            item = strtok(t1,",");
            if(strcmp("0",item) == 0 && memaddy == NULL){
                for(int z = 0; z < 7; z++)
                    item = strtok(NULL, ",");
                //ITEM NOW CONTAINS MEMADDY
            // printf("item contains: %s\n", item);
                index = j;
                memaddy = strdup(item);
                //continue;
            }
            //printf("memaddy: %s\n", memaddy);
            if(strcmp("1", item) == 0 && memaddy != NULL)
            {
                for(int z = 0; z < 5; z++)
                    item = strtok(NULL, ",");
            // printf("item2 contains: %s, memaddy: %s\n", item, memaddy);
                if(strcmp(memaddy,item) == 0)
                {
                    //MEMORY ADDY'S ARE SAME. FOUND A PAIR. CONTINUE TO INDEX(INDEX)
                    //dprintf(fd, "%s\n", newlist[index]);
                    //dprintf(fd, "%s\n", newlist[j]);
                    list[count++] = index;
                    list[count++] = j;
                    newlist[index] = "x";
                    newlist[j] = "x";
                    free(memaddy);
                    memaddy = NULL;
                }
                

            }
            free(t1);
        //free t1, memaddy
        }
        if(memaddy != NULL)
        {
            //NOTHING FOUND??
           // printf("nothign found \n");
            free(memaddy);
            memaddy = NULL;
            new = index + 1;
           // printf("set j: %d\n", new);
        }

    }
    if(newlist != NULL)
        free(newlist);
    return count;
}
/*
*lkmalloc - takes in a size, ptr, flags
* mallocs size bytes to for the *ptr
* returns 0 on success
*/
int lkmalloc(u_int size, void **ptr, u_int flags)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);


    if(flags & LKM_UNDER || flags & LKM_OVER){
        size += 8;
    }

    *ptr = malloc(size);
    
    if(*ptr == NULL){
        errno = ENOMEM;
        return -1;
    }

    // LKM_INIT - initalize memory being allocated to 0
    if(flags & LKM_INIT)
    {
        //printf("init_flag\n");
        memset(*ptr, 0, size);
    }

    // LKM_OVER - alloc. 8 more bytes and write 0x5a in upper 8 
    if(flags & LKM_OVER)
    {
        //printf("in lkm over\n");
        *ptr = *ptr + (size - 8);
        memset(*ptr, 0x5a, 8);
        *ptr = *ptr - (size-8);
    }

    // LKM_UNDER - alloc. 8 more bytes and write 0x6b in lower 8
    if(flags & LKM_UNDER)
    {
        //printf("in lkm under\n");
        under_malloc[und_malloc_count++] = (len_reports);
        memset(*ptr, 0x6b, 8);
        *ptr = *ptr + 8;
        //printf("under_malloc:%d\n", under_malloc[und_malloc_count-1]);
    }


    // ADDING REPORT TO THE LIST OF REPORTS
    int strlen = 500;
    reports[len_reports] = malloc(strlen * sizeof(char));

    char address[255];
    snprintf(address,sizeof(address), "0,%s,%s,%d,%ld.%ld,%d,%d,%p", file_name,fxn_name,line_num,tv.tv_sec,tv.tv_usec,0,size, *ptr);
    memcpy(reports[len_reports++], address, sizeof(address));

    return 0;
}
/*
*lkfree - takes in  ptr, flags
* frees ptr
* returns 0 on success
*/
int lkfree(void **ptr, u_int flags)
{
    int ret = 0;
    // stores the hex value of flags 
    char hex[5];
    sprintf(hex, "0x%x", flags);
 

    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    int strlen = 500;
    reports[len_reports] = malloc(strlen * sizeof(char));

    char address[255];
    if(*ptr == NULL){
        errno = EINVAL;
        ret = EINVAL;
        goto out;
    }
    snprintf(address, sizeof(address), "%p", *ptr);

 

    //LKF_REG - free only if the ptr passed was exactly as alloc
    // if(!flags & !LKF_REG)
    // { // free only if the ptr passed was exactly as was allocated
        int found = 0;
        for(int i = 1; i < len_reports; i++){
            char *t1 = strdup(reports[i]);
            char *item = strtok(t1, ",");
            if(strcmp("0", item) == 0)
            {//MALLOC RECORD, CHECK ALLOC_ADDR_RETURNED 
                for(int i = 0; i < 7; i++)
                    item = strtok(NULL, ",");
                //NOW ITEM CONTAINS THE ALLOC_ADDR_RETURNED ADDRESS. COMPARE WITH PTR ADDRESS
                if(strcmp(address, item) == 0)
                    found = 1;
            }
            free(t1);
        }
        if(found == 0){  //memory address record not found in reports list
            ret = 36;

            if(!flags & !LKF_REG){
                ret = 46;
                errno = EINVAL;
            }
        }
        
    //}

    //int found = 0;
    char *t1, *item;
    int size = 0;
    //if(ret == 0 || ret == 36 )
    for(int i = 1; i < len_reports; i++){
        t1 = strdup(reports[i]);
        item = strtok(t1, ",");
        if(strcmp("0", item) == 0)
        {//MALLOC RECORD, CHECK ALLOC_ADDR_RETURNED 
            for(int i = 0; i < 6; i++)
                item = strtok(NULL, ",");
            size = atoi(item);
            for(int i = 0; i < 1; i++)
                item = strtok(NULL, ",");
            
            

            //NOW ITEM CONTAINS THE ALLOC_ADDR_RETURNED ADDRESS. COMPARE WITH PTR ADDRESS

            int real_address = 0;// = atoi(item);
            int ptr_address = 0;
            sscanf(item,"%x", &real_address);
            sscanf(address,"%x", &ptr_address);
            if((ptr_address - real_address) != 0)
                if(ptr_address > real_address){
                    //printf("ptr_address: %s, real_address: %s, size: %d\n", address, item, size);
                    if((ptr_address - real_address) <= size && real_address == (ptr_address - (ptr_address - real_address))  )
                    {
                        if(LKF_APPROX & flags){
                            int z = (ptr_address - real_address);
                            *ptr = *ptr - z;
                             //printf("LKF_APPROX. ptr addy: %s\n", address);
                             //printf("difference %d\n",(ptr_address - real_address) );
                             //printf("new addy: %p\n", *ptr);

                            //LKF_WARN - print Fa warning if freeing with LKF_APPROX
                            if(flags & LKF_WARN)
                            {
                                fprintf(stderr,"LKF_WARN: Trying to free something in the middle of an allocated memory chunk. LKF_APPROX fixed memory address.\n");

                            }
                            ret = 50;
                        }
                        else{
                            //SHOULD BE FIXING MEMORY ADDRES BUT LKF_APPROX FLAG IS NOT ENABLED.
                            ret = 100 + (ptr_address - real_address);
                        }
                    }
                }
        }
        free(t1);
    }
    if(ret == 46)//STILL NOTHING FOUND, MUST HAVE NEVER BEEN ALLOC'D
    {
        ret = 36;
    }
    if(ret == 0){
        int extrafree = 0;
        for(int i = 0; i < und_malloc_count; i++)
        {
            //printf("und c: %s\n", address);
            if(strstr(reports[under_malloc[i]], address) != NULL)
            {
                extrafree = 1;
                //printf("found.\n");
            }
        }
        snprintf(address,sizeof(address), "1,%s,%s,%d,%ld.%ld,%p,%d,%s,()",file_name,fxn_name,line_num, tv.tv_sec,tv.tv_usec,*ptr, ret, hex);
        memcpy(reports[len_reports++], address, sizeof(address));
   
        if(extrafree == 1)
            *ptr = *ptr - 8;
 
        free(*ptr);
    }
    else //if (ret == 56 || ret == 50)
    {
        //printf("re2t: %d\n", ret);
        if(ret == 50){
            //ret = 0;
            char *passed_addr = strdup(address);
            snprintf(address,sizeof(address), "1,%s,%s,%d,%ld.%ld,%s,%d,%s,()",file_name,fxn_name,line_num, tv.tv_sec,tv.tv_usec,passed_addr, ret, hex);
            memcpy(reports[len_reports++], address, sizeof(address));
            free(passed_addr);
            // if(strstr(reports[len_reports-1], address)){
            //     *ptr = *ptr - 8;
            // }
            free(*ptr);
            ret = 0;
        }
        else if(ret == 46 || ret > 100)
        {
           // snprintf(address,sizeof(address), "MEMORY LEAK-tried to free an allocation with a ptr in the middle of an allocated block.");
             snprintf(address,sizeof(address), "1,%s,%s,%d,%ld.%ld,%p,%d,%s,()",file_name,fxn_name,line_num, tv.tv_sec,tv.tv_usec,*ptr, ret, hex);
            memcpy(reports[len_reports++], address, sizeof(address));
            if(flags & LKF_ERROR)
            {
                fprintf(stderr,"LKF_ERROR - EINVAL. Exiting. (1)\n");
                exit(1);
            }
        }
        else if(ret == 36){
            ret = EINVAL;
            errno = EINVAL;
            goto out;
        }
    }
    return ret;

    out:
        if( ret == EINVAL)
        {
            if(flags & LKF_UNKNOWN)
            {
                fprintf(stderr,"LKF_UNKNOWN-asked to free a ptr that has never been allocated.\n");
            }
            snprintf(address,sizeof(address), "1,%s,%s,%d,%ld.%ld,%s,%d,%s,()",file_name,fxn_name,line_num, tv.tv_sec,tv.tv_usec,"null", ret, hex);
            memcpy(reports[len_reports++], address, sizeof(address));
            if(flags & LKF_ERROR)
            {
                fprintf(stderr,"LKF_ERROR - EINVAL. Exiting. (1)\n");
                exit(1);
            }
            
            return ret;
        }
        if (ret == 46)
        {
            snprintf(address,sizeof(address), "MIDDLE_ALLOC_LEAK,1,%s,%s,%d,%ld.%ld,%p,%d,%s,()",file_name,fxn_name,line_num, tv.tv_sec,tv.tv_usec,*ptr, ret, hex);
            memcpy(reports[len_reports++], address, sizeof(address));
            return ret;
        }
        else{
            printf("\nother error. Handle this\n");
        }

        return ret;
}

int lkreport(int fd, u_int flags)
{
    // dprintf(fd, "\n                                ------- lkreport fd: %d ------- ,\n", fd);

    if(!flags & !LKR_NONE)
    {
        for(int i = 0; i < len_reports; i++)
        {
            free(reports[i]);
        }
    }
    else
    {
        //dprintf(fd, "NONE: %d, SERIOUS: %d, MATCH: %d, BAD_FREE: %d, ORPHAN_FREE: %d, DOUBLE_FREE: %d, APPROX: %d\n\n", flags & LKR_NONE, flags &LKR_SERIOUS,flags&LKR_MATCH,flags&LKR_BAD_FREE,flags&LKR_ORPHAN_FREE,flags&LKR_DOUBLE_FREE,flags&LKR_APPROX);
        
        if(flags & LKR_APPROX)
        {
            dprintf(fd, "   LKR_APPROX    \n");
            int approx_frees[len_reports];
            int num_approx_frees = find_approx_frees(approx_frees);
            for(int i = num_approx_frees-1; i >= 0; i--)
                dprintf(fd, "%s\n", reports[approx_frees[i]]);
        }
        if(flags & LKR_DOUBLE_FREE)
        {
            dprintf(fd, "   LKR_DOUBLE_FREE    \n");

        }
        if(flags & LKR_ORPHAN_FREE)
        {
            dprintf(fd, "   LKR_ORPHAN_FREE     \n");
            int orphan_frees[len_reports];
            int num_orphan_frees = find_orphan_frees(orphan_frees);
            for(int i = 0; i < num_orphan_frees; i++)
                dprintf(fd, "%s\n", reports[orphan_frees[i]]);
        }

        if(flags & LKR_BAD_FREE)
        {
            dprintf(fd, "   LKR_BAD_FREE     \n");
            int bad_frees[len_reports];
            int num_bad_frees = find_bad_frees(bad_frees);
            for(int i = 0; i < num_bad_frees; i++)
                dprintf(fd, "%s\n", reports[bad_frees[i]]);
        }
        
        if(flags & LKR_MATCH)
        {
             dprintf(fd, "   LKR_MATCH     \n");
             int matches[len_reports];
             int num_matches = find_matches(matches);
             for(int i = 0; i < num_matches; i++)
                 dprintf(fd, "%s\n", reports[matches[i]]);
        }
        if(flags & LKR_SERIOUS)
        {
            dprintf(fd, "   LKR_SERIOUS     \n");
            int malloc_leaks[len_reports];
            int num_malloc_leaks = find_serious_leaks(malloc_leaks);
            for(int i = 0; i < num_malloc_leaks; i++)
                dprintf(fd, "%s\n", reports[malloc_leaks[i]]);
        }
        
        //dprintf(fd, "\n   All    \n");
        for(int i = 0; i < len_reports; i++)
        {
            //dprintf(fd, "%s\n", reports[i]);
            free(reports[i]);
        }
    }
    free(reports);
    return 0;
}