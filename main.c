// illustrate multi stage error handling: v4
// variant of v3 that uses only one goto label.
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  
  int bufsize =  getpagesize(); // probably 4K
  void *buf = NULL;
  char *infile; // get it from args passed to main
  char *outfile; // get it from args passed to main
  int fd1 = -1, fd2 = -1, retval = 0;

  // 1. alloc a buffer
  buf = malloc(bufsize);
  if (buf == NULL) {
    perror("malloc");
    retval = 1; // indicates to caller that malloc failed
    goto out;
  }

  // 2. open infile
  //fd1 = open(infile, O_RDONLY);
  
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