// illustrate multi stage error handling: v4
// variant of v3 that uses only one goto label.
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "functions.h"


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
  int debug1 = -1, debug2 = -1, debug4 = -1, debug16 = -1, debug32 = -1;
  int len = bufsize;
  void *buf = NULL;
  char *infile = NULL; // get it from args passed to main
  char *outfile = NULL; // get it from args passed to main
  char *password; //user specified password
  int fd1 = -1, fd2 = -1, retval = 0;
  int decrypt = -1, encrypt = -1, passfile = -1;
  
  int opt;
  extern char *optarg;
  extern char *optarg;
  extern int optind, opterr, optopt;


  //prints all command line arguments
 /*   for(int i = 1; i < argc; i++)
  {
    printf("Arg[%d]: %s\n",i, argv[i]);
  }  */

  
  
  while((opt = getopt(argc, argv, ":p:hvD:ed")) != -1)  
  {  
    switch(opt)  
    {  
      case 'p':  
          printf("passwordFileName: %s\n", optarg);  
          passfile = 1;
          break;  
      case 'h':  
          fprintf(stderr, "error: a simple usage line on stderr and exit with a non-zero status code.\n"); 
          exit(6); 
          break;  
      case 'v':  
          printf("Version: %s\n", VERSION);  
          break; 
      case 'e':  
          if(decrypt == -1)
            encrypt = 1;
          else{
            perror("Cannot decrpt if encrypt option is provided");
            exit(4);
          }
          
          break;   
      case 'D':  
          printf("-D tag: %s\n", optarg);  
          int x = ~(0 & 0) & atoi(optarg);
          debug1 = x & 0x01;
          debug2 = x & 0x02;
          debug4 = x & 0x04;
          debug16 = x & 0x10;
          debug32 = x & 0x20;
          printf("debug1: %d\n", debug16);
          break;
      case 'd':  
          if(encrypt == -1)
            decrypt = 1;
          else{
            perror("Cannot encrypt if decrypt option is provided");
            exit(5);
          }

          break;    
      case '?':  
          printf("unknown option: %c\n", optopt); 
          exit(7);
          break;  
    }  
  }  

  if(optind < argc)
  {
    for(int j = optind; j < argc; j++)
    {
    /*   if(strcmp("-", argv[j]) == 0)
      {
        if(optind == j)
          fd1 = 0;
        else
          fd2 = 0;
      }
      else */
      {
        if(optind == j)
          infile = argv[j];
        else if (optind+1 == j)
          outfile = argv[j];
      }
    }
  }
  else
  {
    perror("No input file provided?");
    exit(8);
  }

  if(encrypt == 1)
    printf("Encrypt with specified user pw\n");
  else if(decrypt == 1)
    printf("Decrypt with specified user pw\n");


/*   if(passfile == -1)
    password = getpass("Enter password:"); */
    password = "abc";



  if(fd1 == 0)
    printf("Reading input from stdio\n");
  
  if(fd2 == 0)
    printf("reading output from stdio\n");

  // 1. alloc a buffer
  buf = malloc(bufsize);
  if (buf == NULL) {
    perror("malloc");
    retval = 1; // indicates to caller that malloc failed
    goto out;
  }

  // 2. open infile
  fd1 = open(infile, O_RDONLY);
  
  if (fd1 < 0) {
    perror(infile);
    retval = 2; // indicates to caller that open infile failed
    goto out;
  }

  // 3. open outfile
  fd2 = open(outfile, O_WRONLY);
  if (fd2 < 0) {
    perror(outfile);
    retval = 3; // indicates to caller that open outfile failed
    goto out;
  }

  // here is the main code: read/write loop
  // if inside r/w loop, something bad happens, you can do cleanup, set
  // retval, and goto out_close2. Also, if the inner loop completely
  // succeeded, set retval=0, and then goto out_close2.
  // 3. read from file 1
  /* int rv = -1;
  while(rv = read(fd1, buf, len) != 0 | rv <! 0){
    if (rv < 0) { // failed
      perror("read");
      retval = 4; //indicates to caller there was a read error
      goto out;
    }
  } */

  if( readFile(fd1, buf, len) < 0){
    perror("read error");
    goto out;
  }


  if( writeFile(fd2, buf, len) < 0){
    perror("write error");
    goto out;

  }


 out:
  // close fd2
  if (fd2 >= 0)
    close(fd2);
  // close fd1
  if (fd1 >= 0)
    close(fd1);
  // free buf
  if (buf != NULL)
    free(buf);

  exit(retval);

  
}