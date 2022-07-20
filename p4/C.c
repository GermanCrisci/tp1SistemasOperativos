#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>

int main(void){
    sem_t * semA;
    sem_t * semB;
    sem_t * semC;
    sem_t * semX;

    semA = sem_open("/semA", 0);
    semB = sem_open("/semB", 0);
    semC = sem_open("/semC", 0);
    semX = sem_open("/semX", 0);

    printf("Se inicio C\n");

    int shmid = shmget(0xa, 0, 0);
    int *iter = (int *) shmat(shmid, 0, 0); 

    for (int i = 0; i < (*iter); i++)
    {
        sem_wait(semC);
        sem_wait(semX);
            printf("C\n");
            sleep(1);
        sem_post(semA);
        sem_post(semB);
    }   

    shmdt(iter);

    return 1;
}