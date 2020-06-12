
#include "client.h"


extern char *optarg;

/*
* client_socket - takes in path which is the name of a file
* creates & connects the socket referred to by the fd to the specified path
* returns the fd
*/
int client_socket(char *path)
{
    int fd = -1;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        sys_error("socket() error");

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strncpy(server.sun_path, path, sizeof(server.sun_path) - 1);
    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0)
        sys_error("connect() error");

    return fd;
}

/*
* FGETS - wrapper function for fgets, contains error checking
*/
int FGETS(char *buf, int size, FILE *stream)
{
    char *fgetRet = fgets(buf, size, stream);
    if (fgetRet == NULL)
        return 1;
    return 0;
}

/*
* get_type - takes in a type [add,list,kill,susp,cont] 
* and sets the value according to the type using a macro
* returns the val represting the type on success or -1 on failure
*/
int get_type(char *type)
{
    int val = -1;

    if (type == NULL)
        return -1;

    // edit commands to accept lowercase
    if (strcmp(type, "add") == 0)
        val = ADD_PKT;
    if (strcmp(type, "list") == 0)
        val = LISTREQ_PKT;
    if (strcmp(type, "kill") == 0)
        val = KILL_PKT;
    if (strcmp(type, "susp") == 0)
        val = SUSP_PKT;
    if (strcmp(type, "cont") == 0)
        val = CONT_PKT;
    if (strcmp(type, "nice") == 0)
        val = NICE_PKT;
    if (val < 0)
        fprintf(stderr, "Unknown command\n");
    return val;
}

/*
* process_request - takes in the command line arguments and the fd of the server
* uses get_type to get the integer representation of the type and executes accordingle
* writes the response to server_fd 
* returns 0 on success, 1 on failure
*/
int process_request(char *line, int server_fd)
{
    //parse line
    if (strchr(line, '\n') != NULL)
        line = strtok(line, "\n");

    char *token = strtok(line, " ");
    int type = -1;
    // get type of request the user wants to send to server
    if ((type = get_type(token)) < 0)
        return type;

    // allocate memory for a packet
    struct packet *pkt = CALLOC(1,sizeof(struct packet));
    char *argv_address = NULL, *envp_address = NULL, *nice_addr = NULL;

    // set the type of request packet to be sent to server and additional arguments
    switch (type)
    {
        case (LISTREQ_PKT):
            pkt->type = LISTREQ_PKT;
            break;
        case (ADD_PKT):
            pkt->type = ADD_PKT;
            while((token = strtok(NULL, " ")) != NULL)
            { 
                //process additional options
                if(strncmp(token, "argv=", 5) == 0)
                { 
                    //store length of string following '=' character into pkt->field1
                    argv_address = token + 5;
                    pkt->field1 = strlen(argv_address);
                }
                else if(strncmp(token, "envp=", 5) == 0)
                { 
                    //store length of string following '=' character into pkt->field2
                    envp_address = token + 5;
                    pkt->field2 = strlen(envp_address);
                }
                else if(strncmp(token, "nice=", 5) == 0)
                { 
                    //store length of string following '=' character into pkt->field2
                    nice_addr = token + 5;
                    if(atoi(nice_addr) > 19 || atoi(nice_addr) < -20)
                    {
                        fprintf(stderr, "Invalid priority value: must be between -20 and 19\n");
                        return 1;
                    }
                    pkt->field3 = atoi(nice_addr);
                }
            }
            if(argv_address == NULL || pkt->field1 == 0)
            {
                fprintf(stderr, "argv not supplied to ADD command\n");
                return 1;
            }
            break;
        case (KILL_PKT):
            token = strtok(NULL, " "); // get job # to kill
            if(token == NULL || atoi(token) == 0)
            {
                fprintf(stderr, "No job specified with KILL \n");
                return 1;
            }
            if(atoi(token) <= 0)
            {
                fprintf(stderr, "Process number for KILL is <= 0");
                return 1;
            }
            pkt->type = KILL_PKT;
            pkt->field1 = atoi(token);
            break;
        case (SUSP_PKT):
            token = strtok(NULL, " "); // get job # to suspend
            if(token == NULL || atoi(token) == 0)
            {
                fprintf(stderr, "No job specified with SUSP \n");
                return 1;
            }
            if(atoi(token) <= 0)
            {
                fprintf(stderr, "Process number for SUSP is <= 0");
                return 1;
            }
            pkt->type = SUSP_PKT;
            pkt->field1 = atoi(token);
            break;
        case (CONT_PKT):
            token = strtok(NULL, " "); // get job # to continue
            if(token == NULL || atoi(token) == 0)
            {
                fprintf(stderr, "No job specified with CONT \n");
                return 1;
            }
            if(atoi(token) <= 0)
            {
                fprintf(stderr, "Process number for CONT is <= 0");
                return 1;
            }
            pkt->type = CONT_PKT;
            pkt->field1 = atoi(token);
            break;
        case (NICE_PKT):
            token = strtok(NULL, " "); // get job # 
            if(token == NULL || atoi(token) == 0)
            {
                fprintf(stderr, "No job specified with NICE \n");
                return 1;
            }
            if(atoi(token) <= 0)
            {
                fprintf(stderr, "Process number for NICE is <= 0");
                return 1;
            }

            pkt->field1 = atoi(token);

            token = strtok(NULL, " "); // get nice  # 
            if(token == NULL || atoi(token) == 0)
            {
                fprintf(stderr, "No job specified with NICE \n");
                return 1;
            }
            if(atoi(token) > 19 || atoi(token) < -20)
            {
                fprintf(stderr, "Invalid priority value: must be between -20 and 19\n");
                return 1;
            }
            pkt->type = NICE_PKT;
            pkt->field2 = atoi(token);
            break;
    }

    // write the packet to server
    WRITE(server_fd, pkt, sizeof(struct packet));

    // write any argument string for add command
    if(argv_address != NULL)
        WRITE(server_fd, argv_address, pkt->field1); //pkt->field1 contains strlen of argv_address
    
    if(envp_address != NULL)
        WRITE(server_fd, envp_address, pkt->field2); //pkt->field2 contains strlen of envp_address

    // free the allocated pkt
    free(pkt);
    return 0;
}

int main(int argc, char *argv[])
{
    char *path = NULL;
    int opt = 0;
    while ((opt = getopt(argc, argv, "hp:")) != -1)
    {
        switch (opt)
        {
        // h flag to display usage and what flags do
        case 'h':
            fprintf(stdout, "USAGE : ./client [-h] [-p pathname]\n");
            fprintf(stdout, "-h displays the server usage\n");
            fprintf(stdout, "-p pathname specifies the pathname to create a domain socket\n");
            exit(EXIT_SUCCESS);
            break;
        // specify a pathname for domain socket
        case 'p':
            path = optarg;
            break;
        case '?':
            fprintf(stderr, "USAGE : ./client [-h] [-p pathname]\n");
            exit(EXIT_FAILURE);
        }
    }
    // use default domain socket filename
    if (path == NULL)
        path = DEFAULT_PATH;

    //check if file exist
    if (access(path, F_OK) < 0)
    {
        fprintf(stderr, "The domain socket file does not exist\n");
        exit(EXIT_FAILURE);
    }

    // get fd of server to read\write to
    int serverfd = client_socket(path);

    char buf[MAX_INPUT];
    struct packet *resp_pkt = CALLOC(1,sizeof(struct packet));
    fprintf(stdout, "Connected to server\n");
    fprintf(stdout, "PRINT USAGE INFO HERE\n");

    // set stdin to nonblocking
    int flags = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, flags | O_NONBLOCK);

    // get flags of serverfd
    flags = fcntl(serverfd, F_GETFL, 0);
   
    while (1)
    {
        // reset the buffer for userinput
        memset(buf,0,MAX_INPUT);
        
        // get the command from user (stdin)
        if(FGETS(buf, MAX_INPUT, stdin) == 0)
        {
            // process the line and write the pkt and arguments to server
            if (process_request(buf, serverfd) < 0)
                continue;
        }

        // reset the buffer
        memset(buf,0,MAX_INPUT);

        // set the serverfd to non-blocking again
        fcntl(serverfd, F_SETFL, flags | O_NONBLOCK);
        int readRet = 0;

        // read any responses sent from the server
        if((readRet = READ(serverfd,resp_pkt,sizeof(struct packet))) == 0)
            continue;
        if(readRet == -1)
        {
            fprintf(stderr,"Server has terminated\n");
            exit(EXIT_SUCCESS);
        }
        // reset to blocking so we read the correct number of bytes
        fcntl(serverfd, F_SETFL, flags);

        // determine what type of response packet was sent
        switch(resp_pkt->type)
        {
            case(LISTRES_PKT):
                if(resp_pkt->field1 != 0)
                {   
                    //do additional read for the filename to open
                    char filename[MAX_INPUT];
                    memset(filename,0,MAX_INPUT);
                    READ(serverfd,filename,resp_pkt->field1);

                    // open and read file containing the list content
                    int resp_fd = OPEN(filename, O_RDONLY, 0);
                    READ(resp_fd,buf,sizeof(buf));

                    // display the list content
                    printf("%s",buf);
                    close(resp_fd);
                }
                break;
            case(OUTPUT_PKT):
                if(resp_pkt->field1 != 0)
                {   
                    //do additional read for the filename to open
                    char filename[MAX_INPUT];
                    memset(filename,0,MAX_INPUT);
                    READ(serverfd,filename,resp_pkt->field1);

                    // open and read the file containing the output of a program
                    int resp_fd = OPEN(filename, O_RDONLY, 0);
                    READ(resp_fd,buf,sizeof(buf));

                    //display the output of the program
                    printf("%s",buf);
                    close(resp_fd);
                    if(unlink(filename) < 0)
                        perror("unlink error");
                }
                break;
            case(ERR_PKT):
                if(resp_pkt->field1 != 0)
                {   
                    //do additional read for error message and display error message
                    char errorstr[MAX_INPUT];
                    memset(errorstr,0,MAX_INPUT);
                    READ(serverfd,errorstr,resp_pkt->field1);
                    printf("%s",errorstr);
                }
                break;
            case(0):
            // server has terminated
                fprintf(stderr,"Server has terminated\n");
                exit(EXIT_SUCCESS);
                break;
        } 
    }
    exit(EXIT_SUCCESS);
}