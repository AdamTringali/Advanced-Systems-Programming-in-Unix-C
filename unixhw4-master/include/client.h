#include "wrapper.h"
#include "protocol.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>

// maximum length of user input from stdin
#define MAX_INPUT 4096

// default name of domain socket client will try to connect to
#define DEFAULT_PATH "socket_default"