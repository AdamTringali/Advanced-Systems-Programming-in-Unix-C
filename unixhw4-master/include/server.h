#include "wrapper.h"
#include "job.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

// maximum number of clients that can be connected
#define LISTEN_MAX 100

// default domain socket filename to create and listen on
#define DEFAULT_PATH "socket_default"

// name of list filename to be appended to 
#define LISTFILE "listfile.out"