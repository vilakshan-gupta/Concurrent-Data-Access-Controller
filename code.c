#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

char* data;
int MAX_DATA = 1024;

sem_t* wrt;
sem_t* mutex_readCount;
sem_t* rd;
sem_t* mutex_readCountInst;
int readCount = 0;
int readCountInst = 0;


struct data{
    int readstartLoc;
    int trywaitCount;
    int readCountInst;
};

void* writer(void* arg){
    int* i = (int*)arg;
    
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
    ts.tv_nsec += 0;


    int getLock = sem_timedwait(wrt, &ts);
    if (getLock == -1) {
        printf("Writer %d: Timed out\n", *i);
        pthread_exit(0);
    }


    for (int i = 0; i < 2048; i++) {
        for (int j = 0; j < 2048; j++) {
        }
    }
    printf("Writer %d: Writing\n", *i);
    int rand_idx = rand() % MAX_DATA;
    data[rand_idx] = 'w';

    sem_post(wrt);
    pthread_exit(0);
}

void* reader(void* arg){
    int* i = (int*)arg;
    struct data* retVal = (struct data*)malloc(sizeof(struct data));
    
    int trywaitCount = 0;

    sem_wait(mutex_readCount);
    int base_delay_us = 1000;  // 1 millisecond
    int max_delay_us = 16000;  // 16 milliseconds
    int delay_us = base_delay_us;

    while(readCount == 0 && sem_trywait(wrt) == -1){
        trywaitCount++;
        sem_post(mutex_readCount);

        usleep(delay_us);
        delay_us *= 2;
        if (delay_us > max_delay_us)
            delay_us = max_delay_us;

        sem_wait(mutex_readCount);
    }
    readCount++;
    sem_post(mutex_readCount);

    delay_us = base_delay_us;
    while(sem_trywait(rd) == -1){
        trywaitCount++;

        usleep(delay_us);
        delay_us *= 2;
        if (delay_us > max_delay_us)
            delay_us = max_delay_us;
    }

    sem_wait(mutex_readCountInst);
    readCountInst++;
    retVal->readCountInst = readCountInst;
    sem_post(mutex_readCountInst);

    for (int i = 0; i < 2048; i++) {
        for (int j = 0; j < 2048; j++) {
        }
    }

    char* read = (char*) malloc(MAX_DATA * sizeof(char));
    bzero(read, MAX_DATA);
    int rand_idx = rand() % MAX_DATA;
    for(int i = rand_idx; i < MAX_DATA; i++)
        read[i-rand_idx] = data[i];
    
    retVal->readstartLoc = rand_idx;
    retVal->trywaitCount = trywaitCount;


    sem_wait(mutex_readCountInst);
    readCountInst--;
    sem_post(mutex_readCountInst);

    sem_post(rd);

    sem_wait(mutex_readCount);
    readCount--;
    if(readCount == 0)
        sem_post(wrt);
    sem_post(mutex_readCount);

    pthread_exit(retVal);
}
int main(int argc, char* argv[]){
    data = (char*)malloc(MAX_DATA * sizeof(char));
    bzero(data, MAX_DATA);
    for(int i = 0; i < MAX_DATA; i++)
        data[i] = 'a';

    if(argc != 4){
        printf("Usage: ./exe <M> <N> <R>\n");
        exit(1);
    }
    int M, N, R;
    M = atoi(argv[1]);
    N = atoi(argv[2]);
    R = atoi(argv[3]);
 
    // unlinking any previously used semaphore_names
    sem_unlink("wrt");
    sem_unlink("rd");
    sem_unlink("mutex_readCount");
    sem_unlink("mutex_readCountInst");
    wrt = sem_open("wrt", O_CREAT, 0666, 1);
    rd = sem_open("rd", O_CREAT, 0666, R);
    mutex_readCount = sem_open("mutex_readCount", O_CREAT, 0666, 1);
    mutex_readCountInst = sem_open("mutex_readCount", O_CREAT, 0666, 1);

    pthread_t* tidR = malloc(N*sizeof(pthread_t));
    pthread_t* tidW = malloc(M*sizeof(pthread_t));

    pthread_attr_t attrR;
    pthread_attr_init(&attrR);
    pthread_attr_t attrW;
    pthread_attr_init(&attrW);
    pthread_attr_setdetachstate(&attrW, PTHREAD_CREATE_DETACHED);
    

    for(int i = 0; i < M; i++){
        int* index = (int*)malloc(sizeof(int));    
        *index = i;
        pthread_create(&tidW[i], &attrW, writer, (void*)index);
    }
    for(int i = 0; i < N; i++){
        int* index = (int*)malloc(sizeof(int));    
        *index = i;
        pthread_create(&tidR[i], &attrR, reader, (void*)index);
    }

    int maxReadCountInst = 0;
    for(int i = 0; i < N; i++){
        struct data* d;
        pthread_join(tidR[i], (void*)&d);
        // printf("Reader %d: %s, %d, %d\n", i, d->line, d->trywaitCount, d->readCountInst);
        printf("Reader %lu: %d, %d, %d\n", tidR[i], d->readstartLoc, d->trywaitCount, d->readCountInst);
        if(d->readCountInst > maxReadCountInst)
            maxReadCountInst = d->readCountInst;
    }

    printf("MAXIMUM READER INSTANCE: %d\n", maxReadCountInst);

    sem_unlink("wrt");
    sem_unlink("rd");
    sem_unlink("mutex_readCount");
    sem_unlink("mutex_readCountInst");

    return 0;
}
