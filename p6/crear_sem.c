#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <wait.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>

int main(int argc, char *argv[])
{    

    if (argv[1] == NULL)
    {
        printf("Por favor ingrese el numuero de iteraciones como argumento\n");
        return -1;
    }

    int semid = semget(0xa, 4, IPC_CREAT|IPC_EXCL|0600);

    if(errno == EEXIST)
        printf("Ya existe un conjunto de semaforos para la clave 0xa\n");
    else 
        printf("semid = %d\n", semid);

    semctl(semid, 0, SETVAL, 1);
    semctl(semid, 1, SETVAL, 1);
    semctl(semid, 2, SETVAL, 0);
    semctl(semid, 3, SETVAL, 0);

    // me guardo en memoria compartida el numero de iteraciones
    
    int iter = atoi(argv[1]);
    printf("Se iterara %d veces\n", iter);


    int shmid = shmget(0xd, sizeof(int), IPC_CREAT | IPC_EXCL | 0700);
    if (shmid < 0)
    {
        printf("Hubo un error al guardar el numero de iteraciones en memoria compartida\n");
    }
    
    printf("shmid %d \n", shmid);
    int *dir = (int *) shmat(shmid, 0, 0);
    *dir = iter;
    shmdt(dir);
}