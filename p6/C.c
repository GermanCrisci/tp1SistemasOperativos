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
    printf("Se inicio C\n");

    int shmid = shmget(0xa, 0, 0);
    int *iter = (int *) shmat(shmid, 0, 0); 

    for (int i = 0; i < (*iter); i++)
    {
        P(semid, 2);
        P(semid, 3);
            printf("C\n");
            sleep(1);
        V(semid, 0);
        V(semid, 1);
    }   

    shmdt(iter);

    return 1;
}