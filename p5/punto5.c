#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <semaphore.h>
#include <pthread.h>

int f_hiloA();
int f_hiloB();
int f_hiloC();

sem_t semA, semB, semC, semX;
int iter;

int main(int argc, char **argv)
{

    if (sem_init(&semA, 0, 1) < 0)
    {
        printf("Error al crear el semaforo A!");
        return -1;
    }
    if (sem_init(&semB, 0, 1) < 0)
    {
        printf("Error al crear el semaforo B!");
        return -1;
    }
    if (sem_init(&semC, 0, 0) < 0)
    {
        printf("Error al crear el semaforo C!");
        return -1;
    }
    if (sem_init(&semX, 0, 0) < 0)
    {
        printf("Error al crear el semaforo X!");
        return -1;
    }

    if (argv[1] == NULL)
    {
        printf("Por favor ingrese el numero de iteraciones como argumento 1.");
        return -1;
    }

    iter = atoi(argv[1]);

    pthread_t hiloA;
    pthread_t hiloB;
    pthread_t hiloC;

    pthread_create(&hiloA, NULL, (void *) f_hiloA, (void *) NULL);
    pthread_create(&hiloB, NULL, (void *) f_hiloB, (void *) NULL);
    pthread_create(&hiloC, NULL, (void *) f_hiloC, (void *) NULL);

    pthread_join(hiloA, NULL);
    pthread_join(hiloB, NULL);
    pthread_join(hiloC, NULL);

    printf("\nFin main!\n");
    return 1;
}

int f_hiloA()
{
    for (int i = 0; i < (iter * 2); i++)
    {
        sem_wait(&semA);
            printf("A");
        sem_post(&semX);
    }
    return 1;
}

int f_hiloB()
{
    for (int i = 0; i < iter; i++)
    {
        sem_wait(&semB);
        sem_wait(&semX);
            printf("B");
        sem_post(&semA);
        sem_post(&semC);
    }
    return 1;
}

int f_hiloC()
{
    for (int i = 0; i < iter; i++)
    {
        sem_wait(&semC);
        sem_wait(&semX);
            printf("C");
        sem_post(&semA);
        sem_post(&semB);
    }
    return 1;
}
