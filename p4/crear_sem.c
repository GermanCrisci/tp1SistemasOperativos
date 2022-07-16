#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/shm.h>

int main(int argc, char *argv[])
{    

    if (argv[1] == NULL)
    {
        printf("Por favor ingrese el numuero de iteraciones como argumento\n");
        return -1;
    }

    sem_t * semA;
    sem_t * semB;
    sem_t * semC;
    sem_t * semX;

    int valSemA;
    int valSemB;
    int valSemC;
    int valSemX;

    sem_unlink("/semA");
    sem_unlink("/semB");
    sem_unlink("/semC");
    sem_unlink("/semX");

    semA = sem_open("/semA", O_CREAT|O_EXCL, 0644, 1);
    semB = sem_open("/semB", O_CREAT|O_EXCL, 0644, 1);
    semC = sem_open("/semC", O_CREAT|O_EXCL, 0644, 0);
    semX = sem_open("/semX", O_CREAT|O_EXCL, 0644, 0);

    sem_getvalue(semA, &valSemA);
    sem_getvalue(semB, &valSemB);
    sem_getvalue(semC, &valSemC);
    sem_getvalue(semX, &valSemX);

    printf("Valor Semaforo A = %d \n", valSemA);
    printf("Valor Semaforo B = %d \n", valSemB);
    printf("Valor Semaforo C = %d \n", valSemC);
    printf("Valor Semaforo X = %d \n", valSemX);

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