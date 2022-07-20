#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>

int main(void){

    sem_t * semA;
    sem_t * semX;

    semA = sem_open("/semA", 0);
    semX = sem_open("/semX", 0);

    printf("Se inicio A\n");

    int shmid = shmget(0xa, 0, 0);
    int *iter = (int *) shmat(shmid, 0, 0); 

    for (int i = 0; i < (*iter)*2; i++)
    {
        sem_wait(semA);
            printf("A\n");
            sleep(1);
        sem_post(semX);
    }   

    shmdt(iter);

    return 1;
}