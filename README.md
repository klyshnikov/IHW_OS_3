 # Клыников Михаил Игоревич
 ## 2 вариант
 ### Требования на 4-5 баллов
 В рамках решеиня задачи сущестует 2 клиента - рой пчел и Винни Пух (для краткости он везде называется просто медведем). Сервер принимает информацию от клиентов и считает,
 сколько ложек мёда набралось. Если достаточно - отправляет запрос Винни Пуху и обнуляет счетчик. Информация о том, кто что делает отображается в консоли. Разберем детально:

Пчелы:
 
Изначально инициализируем сокет для связи с сервером.

```c
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family      = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port        = htons(echoServPort);


    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

    echoStringLen = strlen(echoString);
```

Далее инициализируем нужное кол-во потоков, которые выполняют функцию bee()
```c
    pthread_t bees[N];
    int bee_ids[N];
    for (int i = 0; i < N; i++) {
        bee_ids[i] = i;
        pthread_create(&bees[i], NULL, bee, &bee_ids[i]);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(bees[i], NULL);
    }
```

Функция отправляет серверу информацию о том, что пчела принесла ложку меда в бочку.
```c
void* bee(void* args) {
    while (1) {
        sem_wait(&mutex);
        puts("Bee put a drop of honey in a barrel");
        send(sock, echoString, echoStringLen, 0);
        sem_post(&mutex);
        sleep(rand() % BEE_WORKING_TIME);
    }
}
```

Винни Пух:
Инициализация такая же. Основная функция принимает запрос от сервера о том, что медведя надо бы разбудить 
```c
void* bear(void* args) {
    while (1) {
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        puts("Winnie Pooh wake up and eat honey");
        sleep(1);
    }
}
```

Сервер:

В дополнение сервер соединяется с 2 клиентами подряд
```c
    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,
                           &clntLen)) < 0)
        DieWithError("accept() failed");

    if ((bearSock = accept(servSock, (struct sockaddr *) &echoClntAddr,
                           &bearLen)) < 0)
        DieWithError("accept() failed");
```
Обработчик получает сигналы от пчел и отправляет сигнал медведю, если его надо разбудить
```c
void HandleTCPClient(int clntSocket, int bearSocket)
{
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;

    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");
    //printf("%d", count);

    ++count;
    if (count > H) {
        send(bearSocket, "yes", 3, 0);
        count = 0;
    }
}
```
