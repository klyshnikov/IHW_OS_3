#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define N 5
#define H 10
#define MAXPENDING 5
#define RCVBUFSIZE 32

int count = 0;

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void HandleTCPClient(int clntSocket, int bearSocket, int listenerSocket)
{
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;

    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");
    //printf("%d", count);

    ++count;
    // Sent to listener
    send(listenerSocket, "bee", 3, 0);
    if (count > H) {
        // Sent to listener
        send(listenerSocket, "winnie", 6, 0);
        send(bearSocket, "yes", 3, 0);
        count = 0;
    }
}

int main(int argc, char *argv[])
{
    int servSock;
    int clntSock;
    int bearSock;
    int listenerSock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    struct sockaddr_in echoListenerAddr;
    unsigned short echoServPort;
    unsigned int clntLen;
    unsigned int bearLen;
    unsigned int listenerLen;

    if (argc != 2)
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);

    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    printf("Server IP address = %s. Wait...\n", inet_ntoa(echoClntAddr.sin_addr));

    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,
                           &clntLen)) < 0)
        DieWithError("accept() failed");

    if ((bearSock = accept(servSock, (struct sockaddr *) &echoClntAddr,
                           &bearLen)) < 0)
        DieWithError("accept() failed");

    if ((listenerSock = accept(servSock, (struct sockaddr *) &echoListenerAddr,
                           &listenerLen)) < 0)
        DieWithError("accept() failed");

    while (1)
    {
        clntLen = sizeof(echoClntAddr);

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        HandleTCPClient(clntSock, bearSock, listenerSock);
    }
}

