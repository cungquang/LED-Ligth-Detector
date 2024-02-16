#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "app_helper.h"

#define SERVER_IP "192.168.7.2"
#define SERVER_PORT 12345
#define MAX_BUFFER_SIZE 1500
#define PREV_MESSAGE_SIZE 200

//flag
static bool isTerminated;

//Sokcet setup
static int serverSock;
static int clientSock;
static char previousMessage[PREV_MESSAGE_SIZE];
static int previousMessageSize;

//Thread
static pthread_t udpSever_id;
static pthread_t udpClient_id;

//Declare functions
void *udpServer_thread();
void *udpClient_thread() ;


/*-------------------------- Public -----------------------------*/

void Udp_cleanup() 
{
    if(serverSock) {
        close(serverSock);
    }

    if(clientSock) {
        close(clientSock);
    }

    if(udpSever_id) {
        pthread_join(udpSever_id, NULL);
    }

    if(udpClient_id) {
        pthread_join(udpClient_id, NULL);
    }
}

void Udp_setTerminate(bool terminate_flag) {
    isTerminated = terminate_flag;
}

void Udp_initServer(bool terminate_flag)
{
    Udp_setTerminate(terminate_flag);

    //Create thread
    if(pthread_create(&udpSever_id, NULL, udpServer_thread, NULL) != 0){
        exit(EXIT_FAILURE);
    }
}

void Udp_initClient(bool terminate_flag)
{
    Udp_setTerminate(terminate_flag);

    //Create thread
    if(pthread_create(&udpClient_id, NULL, udpClient_thread, NULL) != 0){
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

    while(!isTerminated)
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

//Client side, send: history, count, length, dips, help (or ?), stop, <Enter>
void *udpClient_thread() 
{
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    char buffer[MAX_BUFFER_SIZE];

    // Create UDP socket
    if ((clientSock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    while (!isTerminated)
    {
        printf("me: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        if (sendto(clientSock, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, server_len) == -1) {
            perror("Sendto failed");
            exit(EXIT_FAILURE);
        }

        // Receive response from the server
        ssize_t bytes_received = recvfrom(clientSock, (char *)buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &server_len);
        if (bytes_received == -1) {
            perror("Failed to receive message");
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("server: %s\n", buffer);

    }
    
    close(clientSock);
    return NULL;
}