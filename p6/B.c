#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>

void P(int, int);
void V(int, int);

int main(void){
    int semid = semget(0xa, 0, 0);
    printf("Se inicio B\n");

    int shmid = shmget(0xd, 0, 0);
    int *iter = (int *) shmat(shmid, 0, 0); 

    for (int i = 0; i < (*iter); i++)
    {
        P(semid, 1);
        P(semid, 3);
            printf("B\n");
            sleep(1);
        V(semid, 0);
        V(semid, 2);
    }   

    shmdt(iter);

    return 1;
}