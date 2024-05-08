#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <pthread.h>
#include <semaphore.h>

#define N 5
#define H 10
#define BEE_WORKING_TIME 5
#define RCVBUFSIZE 32   /* Size of receive buffer */

int sock;                        /* Socket descriptor */
struct sockaddr_in echoServAddr; /* Echo server address */
unsigned short echoServPort;     /* Echo server port */
char *servIP;                    /* Server IP address (dotted quad) */
char *echoString;                /* String to send to echo server */
char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
unsigned int echoStringLen;      /* Length of string to echo */
sem_t mutex;
int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv()
                                        and total bytes read */

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
    if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n",
               argv[0]);
       exit(1);
    }

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    echoString = argv[2];         /* Second arg: string to echo */

    if (argc == 4)
        echoServPort = atoi(argv[3]); /* Use given port, if any */
    else
        echoServPort = 7;  /* 7 is the well-known port for the echo service */


    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */


    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

    echoStringLen = strlen(echoString);          /* Determine input length */

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

    printf("\n");    /* Print a final linefeed */

    close(sock);
    exit(0);
}
