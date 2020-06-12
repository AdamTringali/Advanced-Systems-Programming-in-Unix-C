
#include "job.h"

// environ variable
extern char **environ;

// the head of a job list
static JOB *head = NULL;

// unique jobs id
static int jobNum = 0;

// number of current running jobs
static int runningJobs = 0;

/*
*   printJobs - printing the list of job records
*/
void printJobs()
{
    JOB *tmp = head;
    // print header information
    fprintf(stdout,"%6s %8s %5s %-20s %-15s %-10s %10s %10s %10s %10s\n","JOB","PID","","COMMAND",
        "ENVP","STATUS","PRIORITY","RETURN","USERCPU","SYSCPU");

    while(tmp != NULL)
    {
        // printing each job information
        fprintf(stdout,"%6d %8d %5s %-20s %-15s %-10s %10d %10d %12f %12f\n",tmp->jobNum,tmp->pid,"",
            tmp->argv,tmp->envp,tmp->status,tmp->priority,tmp->retVal,tmp->usrtime,tmp->systime);
        tmp = tmp->next;
    }
}
/**
 * 
 * updating the status of a process in the job list (running,stopped,suspended)
 */
void updatePidStatus(pid_t pid,char *status)
{
    JOB *tmp = head;
    while(tmp != NULL)
    {
        if(tmp->pid == pid)
        {
            tmp->status = status;
            break;
        }
        tmp = tmp->next;
    }
}

/**
 * 
 *  updating the return value of a process 
 */
void updatePidRetVal(pid_t pid, int retVal)
{
    JOB *tmp = head;
    while(tmp != NULL)
    {
        if(tmp->pid == pid)
        {
            tmp->retVal = retVal;
            break;
        }
        tmp = tmp->next;
    }
}
/**
 * 
 *  returning the pid value of a job number
 */
pid_t getPid(int jobNum)
{
    JOB *tmp = head;
    while(tmp != NULL)
    {
        if(tmp->jobNum == jobNum)
        {
            return tmp->pid;
        }
        tmp = tmp->next;
    }
    return -1;
}
/**
 * 
 *  returning the output file of a process
 */
char *getOutFile(pid_t pid)
{
    JOB *tmp = head;
    while(tmp != NULL)
    {
        if(tmp->pid == pid)
        {
            return tmp->outfile;
        }
        tmp = tmp->next;
    }
    return NULL;
}
/**
 * 
 *  updating the usertime and system time of a process
 */
void updatePidTime(pid_t pid,double usertime,double systime)
{
    JOB *tmp = head;
    while(tmp != NULL)
    {
        if(tmp->pid == pid)
        {
            tmp->usrtime = usertime;
            tmp->systime = systime;
            break;
        }
        tmp = tmp->next;
    }
}
/**
 * 
 *  updating the priority of a process
 */
void updatePidPriority(int pid,int priority)
{
     JOB *tmp = head;
    while(tmp != NULL)
    {
        if(tmp->pid == pid)
        {
            tmp->priority = priority;
            break;
        }
        tmp = tmp->next;
    }
}

/**
 * 
 *  reaping all zombie processes (programs that have completed but not reaped)
 *  if there are no zombie process, then we don't wait. A response packet
 *  is sent to the client that the program has completed or terminated and the 
 *  client can read the file containing the output.
 */
void cleanJobs(int clientfd)
{
    // check if any process have completed, if no process is completed dont wait.
    int status;
    struct rusage buf;
    pid_t pid = wait4(-1,&status, WUNTRACED| WNOHANG,&buf);
    // no current child process
    if(pid < 1)
        return;
    // zombie process exists
    while(pid > 0)
    {
        // if child process terminated normally (exit\return)
        if(WIFEXITED(status))
        {
            runningJobs = runningJobs - 1;
            
            double usertime =  (buf.ru_utime.tv_sec)  + (buf.ru_utime.tv_usec/(double)1000000);
            double systime =  (buf.ru_stime.tv_sec)  + (buf.ru_stime.tv_usec/(double)1000000);
            
            // update the job status and return value and time values
            int exitVal = WEXITSTATUS(status);
            updatePidRetVal(pid,exitVal);
            updatePidStatus(pid,COMPLETED);
            updatePidTime(pid,usertime,systime);

            // write a response to the client that the job has completed and for the
            // client to read the output file
            struct packet *pkt = CALLOC(1,sizeof(struct packet));
            pkt->type = OUTPUT_PKT;
            char *file = getOutFile(pid);
            if(file != NULL)
                pkt->field1 = strlen(file);
            WRITE(clientfd, pkt, sizeof(struct packet));
            if(file != NULL)
                WRITE(clientfd, file, pkt->field1);
            free(pkt);

        }
        // if child process terminated by signal
        else if(WIFSIGNALED(status))
        {
            runningJobs = runningJobs - 1;
            double usertime = (buf.ru_utime.tv_sec)  + (buf.ru_utime.tv_usec/(double)1000000);
            double systime =  (buf.ru_stime.tv_sec)  + (buf.ru_stime.tv_usec/(double)1000000);
         
            int signalRet = WTERMSIG(status);
            // updating the return value, status and time of a stopped job
            updatePidRetVal(pid,signalRet);
            updatePidStatus(pid,STOPPED);
            updatePidTime(pid,usertime,systime);

            // write a response to the client that the job has stopped and for the
            // client to read the output file
            struct packet *pkt = CALLOC(1,sizeof(struct packet));
            pkt->type = OUTPUT_PKT;
            char *file = getOutFile(pid);
            if(file != NULL)
                pkt->field1 = strlen(file);
            WRITE(clientfd, pkt, sizeof(struct packet));
            if(file != NULL)
                WRITE(clientfd, file, pkt->field1);
            free(pkt);
        }
        // if child process suspended by signal
        else if(WIFSTOPPED(status))
        {
            runningJobs = runningJobs - 1;
            updatePidStatus(pid,SUSPENDED);
        }
        //check if there are more zombie processes
        pid =  wait4(-1,&status, WUNTRACED| WNOHANG,&buf);
    }
}
/**
 * 
 *  child process is created which constructs argv,envp to be executed.
 *  priority of process is set and output is redirected to a file
 */
pid_t launchJob(int jobNum,char *argv,char *envp,int priority)
{
    int pid = FORK();

    if(pid == 0)
    {

        // redirect stdout and stdin output to file
        char *outputfile = CALLOC(1,20);
        sprintf(outputfile,"%d",getpid());
        strcat(outputfile,"testing.out");
        int outfd = OPEN(outputfile,O_RDWR | O_CREAT,00777);
        DUP2(outfd,1);
        DUP2(outfd,2);

        //format argv and envp to be an array of strings
        char **argv_array = CALLOC(20,sizeof(char *));
        char **envp_array = CALLOC(20,sizeof(char *));
        int argc = 0;
        int envc = 0;
        char *token = strtok(argv,",");
        // parse through argv delimintor ","
        while(token != NULL)
        {
            argv_array[argc] = token;
            argc = argc + 1;
            token = strtok(NULL,",");
        }
        // parse through envp delimintor ","
        token = strtok(envp,",");
        while(token != NULL)
        {
            envp_array[envc] = token;
            envc = envc + 1;
            token = strtok(NULL,",");
        }
        // add envp variables
        for(int i = 0 ; i < envc; i++)
        {
            char *val = strchr(envp_array[i],'-');
            if(val == NULL)
            {
                fprintf(stderr,"Invalid envp argument format\n");
                exit(EXIT_FAILURE);
            }
            *val = '=';
            if(putenv(envp_array[i]) < 0)
            {
                fprintf(stderr,"Unable to add environment string\n");
                exit(EXIT_FAILURE);
            }
        }

        // null terminate argv
        argv_array[argc] = NULL;
        
        // set priority of program
        if(priority != 0)
            NICE(priority);

        // execute program
        execvpe(argv_array[0],argv_array,environ);
        sys_error("execvpe() error");
    }
    // child pid value returned to parent
    return pid;
}

/**
 * 
 *  add job - constructs a job record for the add command added to the list
 *  returns an ERR_PKT if unable to run job due to execeeding the cap
 */
void addJobs(char *argv,char *envp,int priority,int clientfd)
{

    // check number of current jobs running
    if(runningJobs == MAXJOB)
    {
        // construct ERR_PKT to be sent to client along with error message
        struct packet *pkt = CALLOC(1,sizeof(struct packet));
        pkt->type = ERR_PKT;
        pkt->field1 = strlen(MAXJOBERR);
    
        WRITE(clientfd, pkt, sizeof(struct packet));
        WRITE(clientfd, MAXJOBERR, pkt->field1);

        free(pkt);
        return;
    }
    // increment job count and running job count
    runningJobs = runningJobs + 1;
    jobNum = jobNum + 1;

    // construct job record
    JOB * job = CALLOC(1,sizeof(JOB));
    job->jobNum = jobNum;
    job->argv = argv;
    job->envp = envp;
    job->status = RUNNING;
    job->priority = priority;
    job->next = NULL;
    JOB *tmp = head;
    if(tmp != NULL)
    {
        job->next = tmp;
        head = job;
    }
    else
    {
        head = job;
    }

    // launch job and save pid value
    job->pid = launchJob(jobNum,argv,envp,priority);

    // store filename of output of the job
    char *outputfile = CALLOC(1,30);
    sprintf(outputfile,"%d",job->pid);
    strcat(outputfile,"testing.out");
    job->outfile = outputfile;

}

/**
 * 
 *  killJob - stopps a job number stored in field1 of a request packet from client
 */
void killJob(int field1)
{
    pid_t pid = -1;
    if((pid = getPid(field1)) < 0)
    {
        fprintf(stderr,"Invalid job number\n");
        return;
    }
    if(kill(pid,SIGINT) < 0)
        perror("kill() error");
}

/**
 * 
 *  suspJob - suspends a job number stored in field1 of a request packet from client
 */
void suspJob(int field1)
{
    pid_t pid = -1;
    if((pid = getPid(field1)) < 0)
    {
        fprintf(stderr,"Invalid job number\n");
        return;
    }
    if(kill(pid,SIGTSTP) < 0)
        perror("kill() error");
}
/**
 * 
 *  changes the priority of a job number(field1) with value (field2)
 */
void niceJob(int field1,int field2)
{
    pid_t pid = -1;
    if((pid = getPid(field1)) < 0)
    {
        fprintf(stderr,"Invalid job number\n");
        return;
    }
    if(setpriority(PRIO_PROCESS, pid, field2) < 0)
        perror("setpriority() error");
    updatePidPriority(pid,field2);
}

/**
 *  
 *  contJob - continues a job number stored in field1 of a request packet from client.
 *  If max concurrent job, unable to continue running the job.
 */
void contJob(int field1)
{
    if(runningJobs == MAXJOB)
        return;
    runningJobs = runningJobs + 1;
    pid_t pid = -1;
    if((pid = getPid(field1)) < 0)
    {
        fprintf(stderr,"Invalid job number\n");
        return;
    }
    if(kill(pid,SIGCONT) < 0)
        perror("kill() error");

    updatePidStatus(pid,RUNNING);
}








