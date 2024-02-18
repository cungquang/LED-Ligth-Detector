#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "app_helper.h"
#include "app_sampler.h"

#define SERVER_IP "192.168.7.2"
#define SERVER_PORT 12345
#define MAX_BUFFER_SIZE 1500
#define PREV_MESSAGE_SIZE 200

//flag
static int *isTerminated;

//Sokcet setup
static int serverSock;
static char previousMessage[PREV_MESSAGE_SIZE];
static int previousMessageSize;

//Thread
static pthread_t udpSever_id;

//Declare functions
void *udpServer_thread();
const char *command_help(void);
const char *command_unsupport(void);
void command_stop(void);
int command_count(void);
int command_dips(void);
long long command_length(void);

/*-------------------------- Public -----------------------------*/

void Udp_join(void)
{
    pthread_join(udpSever_id, NULL);
}

void Udp_cleanup(void) 
{
    if(serverSock) {
        close(serverSock);
    }

    isTerminated = NULL;
    memset(previousMessage, 0, sizeof(previousMessage));
}

void Udp_initServer(int *terminate_flag)
{
    isTerminated = terminate_flag;

    //Create thread
    if(pthread_create(&udpSever_id, NULL, udpServer_thread, NULL) != 0){
        exit(EXIT_FAILURE);
    }
}


/*-------------------------- Private -----------------------------*/

//Server side, receive: history, count, length, dips, help (or ?), stop, <Enter>
void *udpServer_thread()
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int recv_len;
    char receiv_buffer[MAX_BUFFER_SIZE];

    // Create a UDP socket
    if ((serverSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup receiver addr
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);            //receive from any ip - later need to specify with host IP

    // Bind the socket to Receiver address
    if (bind(serverSock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("Server starting...");
    while(isTerminated == 0)
    {
        // Receive message
        if ((recv_len = recvfrom(serverSock, receiv_buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len)) == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        //Add null terminate
        receiv_buffer[recv_len] = '\0'; 
        
        // Message is not empty -> update previous; otherwise leave it as is
        if(recv_len != 1)
        {
            memset((char *)&previousMessage, 0, sizeof(previousMessage));
            memcpy(previousMessage, receiv_buffer, recv_len - 1);
            previousMessageSize = recv_len;
            previousMessage[previousMessageSize - 1] = '\0';
        }
        
        // Execute command according to request from client
        if(strcmp("help", previousMessage) == 0 || strcmp("?", previousMessage) == 0)
        {
            command_help();
        } 
        else if (strcmp("stop", previousMessage) == 0)
        {
            command_stop();
        }
        else if (strcmp("dips", previousMessage) == 0)
        {
            command_dips();
        }
        else if (strcmp("lenth", previousMessage) == 0)
        {
            command_length();
        }
        else if (strcmp("count", previousMessage) == 0)
        {
            command_count();
        }
        else
        {
            command_unsupport();
        }

        // Print received message
        //printf("%s:%d - say with %d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), recv_len, receiv_buffer);

        // Reply to the sender
        if(sendto(serverSock, previousMessage, previousMessageSize, 0, (struct sockaddr *)&client_addr, client_len) == -1)
        {
            perror("Fail to send");
            exit(EXIT_FAILURE);
        }
    }

    // Close the socket
    close(serverSock);
    return NULL;
}

const char *command_help(void)
{
    return "Accepted command examples:\n"
           "count\t\t-- get the total number of samples taken.\n"
           "length\t\t-- get the number of samples taken in the previously completed second.\n"
           "dips\t\t-- get the number of dips in the previously completed second.\n"
           "history\t\t-- get all the samples in the previously completed second.\n"
           "stop\t\t-- cause the server program to end.\n"
           "<enter>\t\t-- repeat last command.\n";
}

const char *command_unsupport(void)
{
    return "Command is unsupported. Please type \"help\" or \"?\" for supporting command\n";
}

void command_stop(void)
{
    *isTerminated = 1;
}

int command_count(void)
{
    return Sampler_getHistorySize();
}

int command_dips(void)
{
    return Sampler_getDips();
}

long long command_length(void) 
{
    return Sampler_getNumSamplesTaken();
}