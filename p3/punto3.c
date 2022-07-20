#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <pthread.h>

void f_hiloA(void*);
void f_hiloB(void*);
void f_hiloC(void*);

pthread_mutex_t A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t B = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t C = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t X = PTHREAD_MUTEX_INITIALIZER;

int iteraciones;
int terminoC;

int main(int argc, char *argv[])
{
    if (!argv[1])
    {
        printf("Porfavor ingrese un numero de iteraciones como argumento\n");
        return 0;
    }
    
    iteraciones = atoi(argv[1]);
    printf("Se imprimiran %d itreaciones\n", iteraciones);

    
    pthread_t hiloA;
    pthread_t hiloB;
    pthread_t hiloC;
    
    pthread_mutex_unlock(&A);
    pthread_mutex_unlock(&B);
    pthread_mutex_lock(&C);
    pthread_mutex_lock(&X);
    
    pthread_create(&hiloA, NULL, (void *) f_hiloA, (void *) NULL);
    pthread_create(&hiloB, NULL, (void *) f_hiloB, (void *) NULL);
    pthread_create(&hiloC, NULL, (void *) f_hiloC, (void *) NULL);

    pthread_join(hiloA, NULL);
    pthread_join(hiloB, NULL);
    pthread_join(hiloC, NULL);
    
    printf("Fin main!\n");
    return 0;
}

void f_hiloA(void * datos)
{
    while(1){
        pthread_mutex_lock(&A);
            if (terminoC) break;
            printf("A");
        pthread_mutex_unlock(&X);       
    }
    printf("Termino A!\n");
    return;
}

void f_hiloB(void * datos)
{
    while(1){
        pthread_mutex_lock(&B);
        pthread_mutex_lock(&X);
            if (terminoC) break;
            printf("B");
        pthread_mutex_unlock(&A);
        pthread_mutex_unlock(&C);
    }
    printf("Termino B!\n");
    return;
}

void f_hiloC(void * datos)
{
    int i = 0;
    while(!terminoC){
        pthread_mutex_lock(&C);
        pthread_mutex_lock(&X);
            printf("C");
            i++;
            if (i >= iteraciones)
            {
                printf("\n"); // nos aseguramos que terminen los otros hilos.
                terminoC = 1;
                pthread_mutex_unlock(&X);       
            }
        pthread_mutex_unlock(&A);
        pthread_mutex_unlock(&B);

    }
    printf("Termino C!\n");
    return;
}