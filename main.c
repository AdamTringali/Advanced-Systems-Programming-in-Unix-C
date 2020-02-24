// illustrate multi stage error handling: v4
// variant of v3 that uses only one goto label.
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "functions.h"
#include "debug.h"

#define DBG_NONE 0x00
#define DBG_ENTEXIT 001 // entry/exit to fxn
#define DBG_LIB 0x02 // before/after call to lib fxn
#define DBG_SYSCALL 0x04 // before/after call to syscall
#define DBG_ARGS 0x10 // print args before calling fxn
#define DBG_RET	0x20 // print retval before return from fxn

/* 
 TODO:
 len for reading from file ??
 bufsize = getpagesize()??

  preserve outfile on failure?
  same in/out file?
  relative pathnames / absolute ones?

  writeFile in functions.c??
  

*/



#define VERSION "10.2.1"

/* Debug value 0x01 (1d): print a message on immediate entry and right before
  exit to every function in your code that you write, including main(), but
  not library calls you call.  Print the name of the function and whether
  you're entering or exiting it.

- Debug value 0x02 (2d): print right before and right after calling any
  library call (e.g., from libc, libssl, etc.).  Print the function name and
  whether you're before or after calling it.

- Debug value 0x04 (4d): print right before and right after calling any
  system call (e.g., open, read, write, close).  Print the syscall name and
  whether you're before or after calling it.

- Debug value 0x10 (16d): print also arguments to any function for debug
  values 0x1 (upon entry), 0x2 (right before), and 0x4 (right before).

- Debug value 0x20 (32d): print also return values (and errors if any) for
  any function for debug values 0x1 (right before return), 0x2 (right
  after), and 0x4 (right after). */

int main(int argc, char** argv)
{  
  int bufsize = getpagesize(); // probably 4K
  int len = bufsize;
  void *buf = NULL, *preservebuf = NULL;
  char *infile = NULL, *outfile = NULL, *password = NULL;
  int fd1 = -1, fd2 = -1, fd3 = -1, fd4 = -1, retval = 0;
  int decrypt = -1, encrypt = -1;
  int debug1 = -1, debug2 = -1, debug4 = -1, debug16 = -1, debug32 = -1;
  int opt;
  extern char *optarg;
  extern char *optarg;
  extern int optind, opterr, optopt;
  
  while((opt = getopt(argc, argv, ":p:hvD:ed")) != -1)  
  {  
    switch(opt)  
    {  
      case 'p':  
          debug("passwordFileName: %s\n", optarg);  
          password = optarg;
          break;  
      case 'h':  
          fprintf(stderr, "error: a simple usage line on stderr and exit with a non-zero status code.\n"); 
          retval = 14;
          goto out; 
          break;  
      case 'v':  
          debug("Version: %s\n", VERSION);  
          break; 
      case 'e':  
          if(decrypt == -1)
            encrypt = 1;
          else{
            debug("Cannot decrpt if encrypt option is provided");
            retval = 13;
            goto out;
          }
          
          break;   
      case 'D':  
          debug("-D tag: %s\n", optarg);
          char *test; 
          //sscanf(optarg, "%i", &x);
          long x = strtol(optarg, &test, 0);
          debug1 = x & DBG_ENTEXIT;
          debug2 = x & DBG_LIB;
          debug4 = x & DBG_SYSCALL;
          debug16 = x & DBG_ARGS;
          debug32 = x & DBG_RET;
          // printf("debug1: %d\ndebug2: %d\ndebug4: %d\ndebug16: %d\ndebug32: %d\n", debug1, debug2, debug4, debug16, debug32);
          break;
      case 'd':  
          if(encrypt == -1)
            decrypt = 1;
          else{
            debug("Cannot encrypt if decrypt option is provided");
            retval = 12;
            goto out;
          }

          break;    
      case '?':  
          debug("unknown option: %c\n", optopt); 
          retval = 11;
          goto out; 
    }  
  }  

  if(optind < argc)
  {
    for(int j = optind; j < argc; j++)
    {
      if(strcmp("-", argv[j]) == 0)
      {
        if(optind == j)
          fd1 = 0;
        else
          fd2 = 0;
      }
      else 
      {
        if(optind == j)
          infile = argv[j];
        else /* if (optind+1 == j) */
          outfile = argv[j];
      }
    }
  }
  else
  {
    debug("No input file provided\n");
    retval = 10;
    goto out;
  }

  debug("Infile: %s Outfile: %s\n", infile, outfile);

  if(encrypt == 1)
    debug("Encrypt with specified user pw\n");
  else if(decrypt == 1)
    debug("Decrypt with specified user pw\n");


  if(password)
  {
    printf("reading from passfile\n");
  }
  else
    password = getpass("Enter password:"); 
  //password = "abc";

  if(fd1 == 0)
    debug("Reading input from stdio\n");
  
  if(fd2 == 0)
    debug("reading output from stdio\n");

  // alloc a buffer
  buf = malloc(bufsize);
  if (buf == NULL) {
    perror("malloc");
    retval = 1; // indicates to caller that malloc failed
    goto out;
  }

  preservebuf = malloc(bufsize);
  if (preservebuf == NULL) {
    perror("malloc");
    retval = 5;
    goto out;
  }

  /* ################## opening files ########################### */

  // open infile
  fd1 = open(infile, O_RDONLY);
  if (fd1 < 0) {
    perror(infile);
    retval = 2; // indicates to caller that open infile failed
    goto out;
  }

  // open outfile
  fd2 = open(outfile, O_RDWR);
  if (fd2 < 0) {
    perror(outfile);
    retval = 3; // indicates to caller that open outfile failed
    goto out;
  }

  //create/open tempfile
  fd3 = open("preserved-outfile", O_CREAT|O_WRONLY, 00700 ) ;
  if (fd3 < 0) {
    perror("tempfile");
    retval = 4; //indicates to caller that temp file failed
    goto out;
  }

  if(password)
  {
    debug("opening passfile\n");
    //open passfile
    fd4 = open(password, O_RDONLY) ;
    if (fd4 < 0) {
      perror("password file");
      retval = 20; //indicates to caller that pass file failed
      goto out;
    }
  }

  int writeval;
  int blocksz;
  int check_partial;
  /* ################## passfile read ########################### */
   while((blocksz = read(fd4, buf, len)) > 0)
  {
     if (blocksz < 0) { // failed
          debug("password read failed blocksz: %d\n", blocksz);
          retval = 21;
          goto out;
        }     
  } 

  debug("password: %s\n", buf );

  free(buf);
  buf = malloc(bufsize);
  if (buf == NULL) {
    perror("malloc");
    retval = 22; // indicates to caller that malloc failed
    goto out;
  }
  /* ################## outfile to preserve-out read/write loop ########################### */

   while((blocksz = read(fd2, preservebuf, len)) > 0)
  {
     if (blocksz < 0) { // failed
          debug("read failed: readToPreserve: %d\n", blocksz);
          retval = 6;
          goto out;
        }

        check_partial = 0;
        while(check_partial < blocksz)
        {
          writeval = write(fd3, preservebuf, blocksz);
          if(writeval < 0)
          {
            retval = 7;
            goto out;
          }
          check_partial = check_partial + writeval;
        }
  } 

  lseek(fd2, 0, SEEK_SET);

  /* ################## infile/outfile read/write loop ########################### */
  while((blocksz = read(fd1, buf, len)) > 0 ){
      if (blocksz < 0) { // failed
        debug("read failed: retval: %d\n", blocksz);
        retval = 8;
        goto out;
      }

      check_partial = 0;
      while(check_partial < blocksz)
      {
        writeval = write(fd2, buf, blocksz);
        if(writeval < 0)
        {
          retval = 9;
          goto out;
        }
        check_partial = check_partial + writeval;
      }       
  }

  //successful exit
  retval = 0;



 out:
  if(retval == 0)
    debug("Successful.\n");
  else
    //debug("retval: %d\n", retval);
  if(retval == 0)
    remove("preserved-outfile");
  else
  {
    remove(outfile);
    rename("preserved-outfile", outfile);
  }
  // close fd2
  if (fd2 >= 0)
    close(fd2);
  // close fd1
  if (fd1 >= 0)
    close(fd1);
  //close fd3
  if (fd3 >= 0)
    close(fd3);
  //close fd4
  if (fd4 >= 0)
    close(fd4);
  // free buf
  if (buf != NULL)
    free(buf);
  // free preservebuf
  if(preservebuf != NULL)
    free(preservebuf);
  exit(retval);

  
}