#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define CLIENT_IP ""
#define SERVER_IP "192.168.7.2"
#define SERVER_PORT "3001"
#define CLIENT_IP ""
#define CLIENT_PORT ""

static int reciever_sock;
static int sendTo_sock;

static pthread_t udpIn_id;
static pthread_t udpOut_id;

void *udpIn_thread();
void *udpOut_thread();

void init_udp()
{
    // Create & start shutdown thread
    if(pthread_create(&udpIn_id, NULL, udpIn_thread, NULL) != 0){
        return 1;
    }
}

void closeSocket() {
    if(reciever_sock) {
        close(reciever_sock);
    }

    if(sendTo_sock) {
        close(sendTo_sock);
    }
}

void *udpIn_thread()
{
    struct sockaddr_in receiver_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    int recv_len;
    char buffer[BUFFER_SIZE];

    // Create a UDP socket
    if ((reciever_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup receiver addr
    memset((char *)&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(PORT_IN);
    receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to Receiver address
    if (bind(reciever_sock, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        // Receive message
        if ((recv_len = recvfrom(reciever_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&sender_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        // Print received message
        buffer[recv_len] = '\0'; 
        printf("%s:%d: %s\n", inet_ntoa(sender_addr.sin_addr), ntohs(sender_addr.sin_port), buffer);
    }

    // Close the socket
    close(reciever_sock);

    return 0;
}

void *udpClient_thread() 
{
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    char buffer[MAX_BUFFER_SIZE];

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // Send a message to the server
    const char *message = "Hello from the client!";
    if (sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *)&server_addr, server_len) == -1) {
        perror("Sendto failed");
        exit(EXIT_FAILURE);
    }

    printf("Message sent to server: %s\n", message);

    // Receive response from the server
    ssize_t bytes_received = recvfrom(sockfd, (char *)buffer, MAX_BUFFER_SIZE, 0,
                                       (struct sockaddr *)&server_addr, &server_len);
    if (bytes_received == -1) {
        perror("Failed to receive message");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received data
    printf("Received response from server: %s\n", buffer);

    close(sockfd);
    return 0;
}