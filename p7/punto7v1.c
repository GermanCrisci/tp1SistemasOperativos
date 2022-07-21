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

int iter;
void P(int, int);
void V(int, int);

int main(int argc, char *argv[])
{
    if (!argv[1])
    {
        printf("Porfavor ingrese un numero de iteraciones como argumento\n");
        return 0;
    }

    int semid = semget(0xa, 4, IPC_CREAT | IPC_EXCL | 0660);
    if (semid == -1)
    {
        printf("Error de creacion de semaforos\n");
    }
    printf("semid = %d\n", semid);

    semctl(semid, 0, SETVAL, 1);
    semctl(semid, 1, SETVAL, 1);
    semctl(semid, 2, SETVAL, 0);
    semctl(semid, 3, SETVAL, 0);

    iter = atoi(argv[1]);
    printf("Se imprimiran %d itreaciones\n", iter);

    pid_t pid;
    // fork hijo B
    pid = fork();
    if (pid == 0)
    {
        for (int i = 0; i < iter; i++)
        {
            P(semid, 1);
            P(semid, 3);
                printf("B");
            V(semid, 0);
            V(semid, 2);
        }
        return 1;
    }

    // fork hijo C
    pid = fork();
    if (pid == 0)
    {
        for (int i = 0; i < iter; i++)
        {
            P(semid, 2);
            P(semid, 3);
                printf("C");
            V(semid, 0);
            V(semid, 1);
        }
        return 1;
    }
    
    for (int i = 0; i < (iter * 2); i++)
    {
        P(semid, 0);
            printf("A");
        V(semid, 3);
    }

    wait(0);
    wait(0);
    semctl(semid, 0, IPC_RMID);
    printf("\nFin main!\n");

    return 0;
}