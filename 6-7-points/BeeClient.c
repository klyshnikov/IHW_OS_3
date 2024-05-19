#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "general.h"

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

void* bee(void* args) {
    while (1) {
        sem_wait(&mutex);
        puts("Bee put a drop of honey in a barrel");
        send(sock, echoString, echoStringLen, 0);
        sem_post(&mutex);
        sleep(rand() % BEE_WORKING_TIME);
    }
}

int main(int argc, char *argv[])
{
    sem_init(&mutex, 0, 1);
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

    // Init bees
    pthread_t bees[N];
    int bee_ids[N];
    for (int i = 0; i < N; i++) {
        bee_ids[i] = i;
        pthread_create(&bees[i], NULL, bee, &bee_ids[i]);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(bees[i], NULL);
    }

    printf("\n");

    close(sock);
    exit(0);
}
