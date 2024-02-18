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
#define MAX_BUFFER_SIZE 1501            // 1500 bytes and 1 bytes for null pointer
#define PREV_MESSAGE_SIZE 200

//flag
static int *isTerminated;

//Sokcet setup
static int serverSock;
static char previousMessage[PREV_MESSAGE_SIZE];
static int previousMessageSize;

//Response message
static const char *responseMessage;

//Thread
static pthread_t udpSever_id;

//Declare functions
void *udpServer_thread();
const char *command_help(void);
const char *command_unsupport(void);
const char *command_stop(void);
const char *command_count(void);
const char *command_dips(void);
const char *command_length(void);
const char *command_history(struct sockaddr_in *client_addr, socklen_t *client_len);

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
    pthread_create(&udpSever_id, NULL, udpServer_thread, NULL);
}


/*-------------------------- Private -----------------------------*/

//Server side, receive: history, count, length, dips, help (or ?), stop, <Enter>
void *udpServer_thread()
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
            responseMessage = command_help();
        } 
        else if (strcmp("stop", previousMessage) == 0)
        {
            responseMessage = command_stop();
        }
        else if (strcmp("dips", previousMessage) == 0)
        {
            responseMessage = command_dips();
        }
        else if (strcmp("length", previousMessage) == 0)
        {
            responseMessage = command_length();
        }
        else if (strcmp("count", previousMessage) == 0)
        {
            responseMessage = command_count();
        }
        else if (strcmp("history", previousMessage) == 0)
        {
            responseMessage = command_history(&client_addr, &client_len);
        }
        else
        {
            responseMessage = command_unsupport();
        }

        // Reply to the sender
        if(responseMessage)
        {
            if(sendto(serverSock, responseMessage, strlen(responseMessage), 0, (struct sockaddr *)&client_addr, client_len) == -1)
            {
                perror("Fail to send");
                exit(EXIT_FAILURE);
            }
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

const char *command_stop(void)
{
    *isTerminated = 1;
    return NULL;
}

const char *command_count(void)
{
    static char command_buffer[MAX_BUFFER_SIZE];             //declare static to keep memory for command buffer
    double num = Sampler_getHistorySize();
    snprintf(command_buffer, sizeof(command_buffer), "%5.3f\n", num);
    return command_buffer;
}

const char *command_dips(void)
{
    static char command_buffer[MAX_BUFFER_SIZE];
    int dips = Sampler_getDips();
    snprintf(command_buffer, sizeof(command_buffer), "%d\n", dips);
    return command_buffer;
}

const char *command_length(void) 
{
    static char command_buffer[MAX_BUFFER_SIZE];     
    long long length = Sampler_getNumSamplesTaken();
    snprintf(command_buffer, sizeof(command_buffer), "%lld\n", length);
    return command_buffer;
}

const char *command_history(struct sockaddr_in *client_addr, socklen_t *client_len)
{
    static char command_buffer[MAX_BUFFER_SIZE];
    int history_size;
    int current_buffer_size;
    double *history = Sampler_getHistory(&history_size);

    for(int i = 0; i < history_size; i++)
    {
        //Convert doubles -> string
        int temp_size;
        const char *temp_response = convertDataToString(&temp_size, history[i]);

        //if fit into current size 
        if(current_buffer_size + temp_size <= MAX_BUFFER_SIZE)
        {
            mergeToBuffer(command_buffer, &current_buffer_size, temp_response, temp_size);
        }
        //if oversize -> send data
        else
        {
            //need to cast type (struct sockaddr *) for client_addr
            sendto(serverSock, command_buffer, strlen(command_buffer), 0, (struct sockaddr *)client_addr, *client_len);

            //reset data
            memset(command_buffer, 0, sizeof(command_buffer));
            current_buffer_size = 0;
        }

        //free each time complete
        free((void *)temp_response);
        temp_response = NULL;
    }

    //Set history = NULL
    history = NULL;

    //Return the last string -> to send to user
    return command_buffer;
}
