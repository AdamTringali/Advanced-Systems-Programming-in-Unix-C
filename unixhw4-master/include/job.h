
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include "wrapper.h"
#include <signal.h> 
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include "protocol.h"

// maximum number of concurrent jobs that can be ran
#define MAXJOB 3

// error string to send to client if trying to add too many jobs
#define MAXJOBERR "UNABLE TO ADD JOB. TOO MANY RUNNING.\n"


// constants for job status
#define RUNNING "Running"
#define SUSPENDED "Suspended"
#define STOPPED "Stopped"
#define COMPLETED "Completed"

// structure of a job
typedef struct job{
    int jobNum;
    pid_t pid;
    char *argv;
    char *envp;
    char *status;
    struct job *next;
    int retVal;
    int priority;
    char *outfile;
    double usrtime;
    double systime;
}JOB;

// job methods (stop,suspend,continue,nice,clean,print)
void printJobs();
void addJobs(char *argv,char *envp,int priority,int clientfd);
// field1 of pkt will contain job number
void killJob(int field1);
void suspJob(int field1);
void contJob(int field1);
// field2 contains nice value
void niceJob(int field1,int field2);
void cleanJobs(int clientfd);