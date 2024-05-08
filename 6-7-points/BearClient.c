#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define N 5
#define H 10
#define BEE_WORKING_TIME 5
#define RCVBUFSIZE 32

int sock;
struct sockaddr_in echoServAddr;
unsigned short echoServPort;
char *servIP;
char *echoString;
char echoBuffer[RCVBUFSIZE];
unsigned int echoStringLen;
int bytesRcvd, totalBytesRcvd;

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void* bear(void* args) {
    while (1) {
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        puts("Winnie Pooh wake up and eat honey");
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    if ((argc < 3) || (argc > 4))
    {
        fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n",
                argv[0]);
        exit(1);
    }

    servIP = argv[1];
    echoString = argv[2];

    if (argc == 4)
        echoServPort = atoi(argv[3]);
    else
        echoServPort = 7;

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family      = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port        = htons(echoServPort);

    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

    echoStringLen = strlen(echoString);


    pthread_t bear_thread;
    int bear_id = 0;
    pthread_create(&bear_thread, NULL, bear, &bear_id);
    pthread_join(bear_thread, NULL);

    printf("\n");

    close(sock);
    exit(0);
}