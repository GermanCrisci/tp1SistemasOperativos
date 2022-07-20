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

int main(int argc, char *argv[])
{
    char* entrada;
    int aux1 = -1;
    int aux2 = -1;
    printf("main(): Cuantos hijos desea crear? ");
    fgets(entrada, 256, stdin);
    // paso cadena a integer.
    num = atoi(entrada);
    printf("main(): Se crearan %d procesos hijos\n", num);
    // creo hijos
    for (i = 1; i <= num; i++)
    {
        pid_t pid = fork();
        if (pid == 0) // hijo creado
        {
            aux1 = pid;
            aux2 = i;
            // le cambio el handler para sigint (2) y sigusr1 (10)
            signal(SIGUSR1, handle_usr_int);
            signal(SIGINT, handle_usr_int);
            break;
        } else { // padre
            printf("main(): Se creo proceso hijo %d con pid = %d\n", i, pid);
        }
    }
    if (aux1 == 0)
    {
        printf("main(): Soy hijo pid: %d esperando una senial, devuelvo %d\n", getpid(), aux2);
        // espera no activa de hijo
        pause();
    }
    
    // cambio el hadle de sigchld (17)
    signal(SIGCHLD, handle_sigchld);    
    printf("main(): Soy padre pid: %d esperando a seniales de mis hijos\n", getpid());
    // espera no activa del padre mientras no terminan los hijos.
    while (hijosFinalizados < num) pause();
}

void handle_sigchld(int signo)
{
    int estado = 0;
    printf("handle_sigchld(): Recibi senial %d\n", signo);
    pid_t pid = wait(&estado);
    printf("handle_sigchld(): Finallizo proceso %d y retorno %d\n", pid, estado/256);
    hijosFinalizados++;
    printf("handle_sigchld(): Hijos finalizados: %d\n", hijosFinalizados);
    suma = suma + estado/256;
    if (hijosFinalizados == num)
    {
        printf("handle_sigchld(): Finalizaron todos los hijos!\n");
        printf("handle_sigchld(): Resultado suma %d!\n", suma);
        exit(suma);
    }
}

void handle_usr_int(int signo)
{
    printf("handle_usr_int(): Soy hijo, recibi senial %d\n", signo);
    // devuelvo el numero de hijo.
    exit(i);
}