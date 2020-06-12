
#include "server.h"

extern char *optarg;

/**
 *  
 *  server_socket - create a domain socket (pipe) with a specified filename (path)
 *  that is binded and listened on for connections (clients).
 */
int server_socket(char *path)
{
    int listenfd = -1;
    // create a unix pipe (domain socket)
    if((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        sys_error("socket() error");

    // bind the fd
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);
    if(bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        sys_error("bind() error");

    // listen on the fd
    if(listen(listenfd,LISTEN_MAX) < 0)
        sys_error("listen() error");
    return listenfd;
}

/**
 *  wrapper for accept system call. that accepts connections
 */
int ACCEPT(int sockfd,struct sockaddr *addr,socklen_t *addrlen)
{
    int fd;
    if((fd = accept(sockfd,NULL,NULL)) < 0)
        sys_error("accept() error");
    return fd;
}
/**
 * 
 *  response_client - sends a response packet to a client given by clientfd,
 *  of a type along with any file argument (this is file of output of a program)
 */
void response_client(int clientfd,int type,char *file)
{
    // allocate memory for a packet and construct the packet.
    struct packet *pkt = CALLOC(1,sizeof(struct packet));
    pkt->type = type;

    // if there are file arguments, store length of file into packet
    if(file != NULL)
        pkt->field1 = strlen(file);

    // write packet to clientfd
    WRITE(clientfd, pkt, sizeof(struct packet));

    // write filename to clientfd
    if(file != NULL)
        WRITE(clientfd, file, pkt->field1);
    free(pkt);
}

void process_client(int clientfd)
{

    // allocate memory for a packet
    PACKET *pkt = CALLOC(1,sizeof(PACKET));

    // get flags of clientfd
    int flags = fcntl(clientfd, F_GETFL, 0);
    while(1)
    {
        // reset memory of pkt (buffer for a packet)
        memset(pkt,0,sizeof(PACKET));

        // set clientfd to be read without blocking
        fcntl(clientfd, F_SETFL, flags | O_NONBLOCK);

        // read from clientfd a request packet
        int readRet = 0;
        if((readRet = READ(clientfd,pkt,sizeof(PACKET))) == -1)
        {
            fprintf(stderr,"Client ended connection\n");
            exit(EXIT_FAILURE);
        }
        // reset clientfd to be blocking again
        fcntl(clientfd, F_SETFL, flags);

        //before processing the packet, check if any process has completed
        cleanJobs(clientfd);

        // process the packet
        switch (pkt->type)
        {
            // process the list command
            case (LISTREQ_PKT):
                debug("LISTREQ PACKET\n");
                
                // store the job list content info a file by redirection
                char *listfile = CALLOC(1,20);
                sprintf(listfile,"%d",getpid());
                strcat(listfile,LISTFILE);
                
                // open file for list and redirect stdout to file
                int listfd = OPEN(listfile,O_RDWR | O_CREAT,00777);
                int stdout_copy = DUP(STDOUT_FILENO);

                fflush(stdout);
                DUP2(listfd,STDOUT_FILENO);

                printJobs();
                fflush(stdout);
              
                DUP2(stdout_copy,STDOUT_FILENO);
                close(listfd);
                close(stdout_copy);

                //write list response to client
                response_client(clientfd,LISTRES_PKT,listfile);
        
                free(listfile);
                break;
            
            // process the add command
            case (ADD_PKT):
                debug("ADD_PKT\n");

                // perform an additional read to retrieve the argv string
                char *argv = CALLOC(1,(pkt->field1)+1);
                READ(clientfd,argv,pkt->field1);

                // check if there is envp string to be retrieved
                char *envp = NULL;
                if(pkt->field2 > 0)
                {
                    envp = CALLOC(1,(pkt->field2)+1);
                    READ(clientfd,envp,pkt->field2);
                }

                // execute adding a job functionality
                addJobs(argv,envp,pkt->field3,clientfd);
                break;
            // processing kill packet
            case (KILL_PKT):
                debug("KILL PACKET\n");
                killJob(pkt->field1);
                break;
            // processing suspend packet
            case (SUSP_PKT):
                debug("SUSP PACKET\n");
                suspJob(pkt->field1);
                break;
            // processing continue packet
            case (CONT_PKT):
                debug("CONT PACKET\n");
                contJob(pkt->field1);
                break;
            case (NICE_PKT):
                debug("NICE PACKET\n");
                niceJob(pkt->field1,pkt->field2);
                break;
        }
    }
}
/**
 *  main for server
 */
int main(int argc,char *argv[]){

    char *path = NULL;
    int opt = 0;
    while((opt = getopt(argc,argv,"hp:")) != -1)
    {
        switch(opt)
        {
            // h flag to display usage and what flags do
            case 'h':
                fprintf(stdout,"USAGE : ./server [-h] [-p pathname]\n");
                fprintf(stdout,"-h displays the server usage\n");
                fprintf(stdout,"-p pathname specifies the pathname to create a domain socket\n");
                exit(EXIT_SUCCESS);
                break;
            // specify a pathname for domain socket
            case 'p':
                path = optarg;
                break;
            case '?':
                fprintf(stderr,"USAGE : ./server [-h] [-p pathname]\n");
                exit(EXIT_FAILURE);
        }
    }
    // use default domain socket filename
    if(path == NULL)
        path = DEFAULT_PATH;
       
    //check if the passed in path or default path exist, if it does, error
    if(access(path,F_OK) != -1)
    {
        fprintf(stderr,"The default domain socket filename alreay exists\n");
        exit(EXIT_FAILURE);
    }

    // set up socket,bind,and listen
    int listenfd = server_socket(path);

    int clients[LISTEN_MAX];
    int num_clients = 0;
    while(1)
    {
        // accept client connections
        int clientfd = ACCEPT(listenfd,NULL,NULL);

        clients[num_clients] = clientfd;
        num_clients = num_clients + 1;

        // fork a process for the connected client
        int pid = FORK();
        if(pid == 0)
            process_client(clientfd);
        fprintf(stderr,"Client connected to server with fd %d\n",clients[num_clients]);
        num_clients = num_clients + 1;
    }
    exit(EXIT_SUCCESS);
}