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
#define PREV_MESSAGE_SIZE 200
#define MAX_BUFFER_SIZE 1501                        // 1500 bytes and 1 bytes for null pointer
#define WRITE_UP_TO (MAX_BUFFER_SIZE - 2)           // Last - \0 & second last - \n

//flag
static int *isTerminated;

//Sokcet setup
static int serverSock;
static char previousMessage[PREV_MESSAGE_SIZE];
static int previousMessageSize;

//Response message
static const char *responseMessage;
static char command_buffer[MAX_BUFFER_SIZE];

//Thread
static pthread_t udpSever_id;

//Declare functions
void *UDP_serverThread();
const char *UDP_commandHelp(void);
const char *UDP_commandUnsupport(void);
const char *UDP_commandStop(void);
const char *UDP_commandCount(void);
const char *UDP_commandDips(void);
const char *UDP_commandLength(void);
const char *UDP_commandHistory(struct sockaddr_in *client_addr, socklen_t *client_len);

/*-------------------------- Public -----------------------------*/

void UDP_join(void)
{
    pthread_join(udpSever_id, NULL);
}

void UDP_cleanup(void) 
{
    if(serverSock) {
        close(serverSock);
    }

    isTerminated = NULL;
    memset(previousMessage, 0, sizeof(previousMessage));
}

void UDP_initServer(int *terminate_flag)
{
    isTerminated = terminate_flag;

    //Create thread
    pthread_create(&udpSever_id, NULL, UDP_serverThread, NULL);
}


/*-------------------------- Private -----------------------------*/

//Server side, receive: history, count, length, dips, help (or ?), stop, <Enter>
void *UDP_serverThread()
{
    printf("Setup server...\n");
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

    //Print server start
    printf("Server starting...\n");

    while(*isTerminated == 0)
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
            responseMessage = UDP_commandHelp();
        } 
        else if (strcmp("stop", previousMessage) == 0)
        {
            responseMessage = UDP_commandStop();
        }
        else if (strcmp("dips", previousMessage) == 0)
        {
            responseMessage = UDP_commandDips();
        }
        else if (strcmp("length", previousMessage) == 0)
        {
            responseMessage = UDP_commandLength();
        }
        else if (strcmp("count", previousMessage) == 0)
        {
            responseMessage = UDP_commandCount();
        }
        else if (strcmp("history", previousMessage) == 0)
        {
            //Send the remaining message
            responseMessage = UDP_commandHistory(&client_addr, &client_len);
        }
        else
        {
            responseMessage = UDP_commandUnsupport();
        }

        // Reply to the sender
        if(responseMessage)
        {
            if(sendto(serverSock, responseMessage, strlen(responseMessage), 0, (struct sockaddr *)&client_addr, client_len) == -1)
            {
                perror("Fail to send");
                exit(EXIT_FAILURE);
            }

            //reset responseMessage
            responseMessage = NULL;
        }
    }

    // Close the socket
    close(serverSock);
    return NULL;
}

const char *UDP_commandHelp(void)
{
    return "Accepted command examples:\n"
           "count\t\t-- get the total number of samples taken.\n"
           "length\t\t-- get the number of samples taken in the previously completed second.\n"
           "dips\t\t-- get the number of dips in the previously completed second.\n"
           "history\t\t-- get all the samples in the previously completed second.\n"
           "stop\t\t-- cause the server program to end.\n"
           "<enter>\t\t-- repeat last command.\n";
}

const char *UDP_commandUnsupport(void)
{
    return "Command is unsupported. Please type \"help\" or \"?\" for supporting command\n";
}

const char *UDP_commandStop(void)
{
    *isTerminated = 1;
    return NULL;
}

const char *UDP_commandCount(void)
{
    //Clear data from previous call
    memset(command_buffer, 0, sizeof(command_buffer));
    double num = SAMPLER_getHistorySize();
    snprintf(command_buffer, sizeof(command_buffer), "%5.3f\n", num);
    return command_buffer;
}

const char *UDP_commandDips(void)
{
    //Clear data from previous call
    memset(command_buffer, 0, sizeof(command_buffer));
    int dips = SAMPLER_getDips();
    snprintf(command_buffer, sizeof(command_buffer), "%d\n", dips);
    return command_buffer;
}

const char *UDP_commandLength(void) 
{
    //Clear data from previous call
    memset(command_buffer, 0, sizeof(command_buffer)); 
    long long length = SAMPLER_getNumSamplesTaken();
    snprintf(command_buffer, sizeof(command_buffer), "%lld\n", length);
    return command_buffer;
}

const char *UDP_commandHistory(struct sockaddr_in *client_addr, socklen_t *client_len)
{
    int temp_size;
    int history_size;
    int current_buffer_size = 0;
    const char *temp_response;

    //Clear data from previous call
    memset(command_buffer, 0, sizeof(command_buffer));
    
    //double *history = Sampler_getHistory(&history_size);
    double *history = SAMPLER_testHistory(&history_size);
    int itemPerLine = 0;

    //UDP Packet should include 20 number per line
    for(int i = 0; i < history_size; i++)
    {
        //Convert doubles -> string
        temp_size = 0;
        temp_response = convertDataToString(&temp_size, history[i]);

        //if fit into current size - need to + 1 for ','
        if(itemPerLine < 20)
        {
            mergeToBuffer(command_buffer, &current_buffer_size, temp_response, temp_size);
            itemPerLine++;
        }
        //if oversize -> send data
        else
        {
            //need to cast type (struct sockaddr *) for client_addr
            command_buffer[current_buffer_size] = '\n';
            sendto(serverSock, command_buffer, strlen(command_buffer), 0, (struct sockaddr *)client_addr, *client_len);

            //reset data
            memset(command_buffer, 0, sizeof(command_buffer));
            current_buffer_size = 0;

            //merge to buffer 
			mergeToBuffer(command_buffer, &current_buffer_size, temp_response, temp_size);
            itemPerLine = 1;
        }

        //free temp_response each time completing merge
        free((void *)temp_response);
        temp_response = NULL;
    }

    //Set history = NULL
    free((void *) history);
    history = NULL;

    //Return the remaining string -> to send to user
    command_buffer[current_buffer_size] = '\n';
    return command_buffer;
}
