#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int iter;

int main(int argc, char *argv[])
{
    if (!argv[1])
    {
        printf("Porfavor ingrese un numero de iteraciones como argumento\n");
        return 0;
    }

    sem_t * semA;
    sem_t * semB;
    sem_t * semC;
    sem_t * semX;

    int valSemA;
    int valSemB;
    int valSemC;
    int valSemX;

    sem_unlink("semA");
    sem_unlink("semB");
    sem_unlink("semC");
    sem_unlink("semX");

    semA = sem_open("semA", O_CREAT | O_EXCL, 0644, 1);
    semB = sem_open("semB", O_CREAT | O_EXCL, 0644, 1);
    semC = sem_open("semC", O_CREAT | O_EXCL, 0644, 0);
    semX = sem_open("semX", O_CREAT | O_EXCL, 0644, 0);

    sem_getvalue(semA, &valSemA);
    sem_getvalue(semB, &valSemB);
    sem_getvalue(semC, &valSemC);
    sem_getvalue(semX, &valSemX);

    printf("Valor Semaforo A = %d \n", valSemA);
    printf("Valor Semaforo B = %d \n", valSemB);
    printf("Valor Semaforo C = %d \n", valSemC);
    printf("Valor Semaforo X = %d \n", valSemX);

    iter = atoi(argv[1]);
    printf("Se imprimiran %d itreaciones\n", iter);

    pid_t pid;
    // fork hijo B
    pid = fork();
    if (pid == 0)
    {
        for (int i = 0; i < iter; i++)
        {
            sem_wait(semB);
            sem_wait(semX);
            printf("B");
            sem_post(semA);
            sem_post(semC);
        }
        return 1;
    }

    // fork hijo C
    pid = fork();
    if (pid == 0)
    {
        for (int i = 0; i < iter; i++)
        {
            sem_wait(semC);
            sem_wait(semX);
            printf("C\n");
            sem_post(semA);
            sem_post(semB);
        }
        return 1;
    }

    for (int i = 0; i < (iter * 2); i++)
    {
        sem_wait(semA);
        printf("A");
        sem_post(semX);
    }

    wait(0);
    wait(0);
    printf("\nFin main!\n");

    return 0;
}