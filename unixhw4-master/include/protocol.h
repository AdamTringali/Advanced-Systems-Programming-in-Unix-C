
/**
 * 
 * Client to Server requests
 * 
 *  ADD - add a job (argv[],envp[],priority)
 *  LIST - list all jobs
 *  KILL - kill a job
 *  SUSP - suspend a job
 *  NICE - change the "nice" priority of a job
 *  
 * 
 * Server to client responses
 *  ERR - error response to maximum concurrent running jobs
 *  OUTPUT_PKT - standard output of a job
 *  LISTRES_PKT - list of all jobs
 * 
 */

typedef struct packet {

    int type;   //  type of request\response
    int field1; //  stores argc or job number
    int field2; //  stores envp
    int field3; //  stores the priority value 

} PACKET;

typedef enum {
    NONE_PKT,
    // client request pkts
    ADD_PKT,LISTREQ_PKT,KILL_PKT,SUSP_PKT,CONT_PKT,NICE_PKT, 
    // server response pkts    
    ERR_PKT,OUTPUT_PKT,LISTRES_PKT                            
}PACKET_TYPE;

