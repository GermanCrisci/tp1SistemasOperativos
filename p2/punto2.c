#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void handle_sigchld(int);
void handle_usr_int(int);
int i;
int hijosFinalizados = 0;
int num;
int suma = 0;
int procesando = 1;

int main(int argc, char *argv[])
{
    char* entrada;
    int aux1 = -1;
    int aux2 = -1;
    printf("Cuantos hijos desea crear?");
    fgets(entrada, 256, stdin);
    num = atoi(entrada);
    printf("Se crearan %d procesos hijos\n", num);
    for (i = 1; i <= num; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            aux1 = pid;
            aux2 = i;
            signal(SIGUSR1, handle_usr_int);
            signal(SIGINT, handle_usr_int);
            break;
        } else {
            printf("Se creo proceso hijo %d con pid = %d\n", i, pid);
        }
    }
    if (aux1 == 0)
    {
        // ciclo infinito de hijo
        do {
            printf("Soy hijo pid: %d esperando una senial, devuelvo %d\n", getpid(), aux2);
            sleep(2);
        } while (procesando);
        exit(aux2);
    }
    signal(SIGCHLD, handle_sigchld);    
    do {
        printf("Soy padre pid: %d esperando a seniales de mis hijos\n", getpid());
        sleep(2);
    } while (procesando);
    exit(suma);
}

void handle_sigchld(int signo)
{
    int estado = 0;
    printf("Recibi senial %d\n", signo);
    pid_t pid = wait(&estado);
    printf("Finallizo proceso %d y retorno %d\n", pid, estado/256);
    hijosFinalizados++;
    suma = suma + estado/256;
    if (hijosFinalizados == num)
    {
        printf("Finalizaron todos los hijos!\n");
        exit(suma);
    }
}

void handle_usr_int(int signo)
{
    printf("Soy hijo, recibi senial %d\n", signo);
    procesando = 0;
    exit(i);
}