#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int iter;
void enviarMsg(int, const char *);
void recibirMsg(int, char *, int);

int main(int argc, char *argv[])
{
    if (!argv[1])
    {
        printf("Porfavor ingrese un numero de iteraciones msgo argumento\n");
        return 0;
    }

    // uso pipes para sincronizar
    int fdA[2], fdB[2], fdC[2], fdX[2];
    pipe(fdA);
    printf("Creado pipe A, %d %d\n", fdA[0], fdA[1]);
    pipe(fdB);
    printf("Creado pipe B, %d %d\n", fdB[0], fdB[1]);
    pipe(fdC);
    printf("Creado pipe C, %d %d\n", fdC[0], fdC[1]);
    pipe(fdX);
    printf("Creado pipe X, %d %d\n", fdX[0], fdX[1]);

    char msg[100]; // comunicacion

    iter = atoi(argv[1]);
    printf("Se imprimiran %d itreaciones\n", iter);

    pid_t pid;
    // fork hijo B
    pid = fork();
    if (pid == 0) // hijo B lee de fdB, fdX escribe en fdA y fdC
    {
        // cierro escritura en B y X
        close(fdB[1]);
        close(fdX[1]);
        // cierro lectura en A y C
        close(fdA[0]);
        close(fdC[0]);

        for (int i = 0; i < iter; i++)
        {
            printf("Leyendo pipe B\n");
            recibirMsg(fdB[0], msg, 100);
            printf("Leyendo pipe X\n");
            recibirMsg(fdX[0], msg, 100);
            printf("B");
            printf("Enviando mensaje a pipe A\n");
            enviarMsg(fdA[1], "Post A\n");
            printf("Enviando mensaje a pipe C\n");
            enviarMsg(fdC[1], "Post C\n");
        }

        close(fdB[0]);
        close(fdX[0]);
        close(fdA[1]);
        close(fdC[1]);

        return 1;
    }

    // fork hijo C
    pid = fork();
    if (pid == 0) // hijo C lee de fdC, fdX escribe en fdA y fdB
    {
        // cierro escritura en C y X
        close(fdC[1]);
        close(fdX[1]);
        // cierro lectura en A y B
        close(fdA[0]);
        close(fdB[0]);

        for (int i = 0; i < iter; i++)
        {
            printf("Leyendo pipe C\n");
            recibirMsg(fdC[0], msg, 100);
            printf("Leyendo pipe X\n");
            recibirMsg(fdX[0], msg, 100);
            printf("B");
            printf("Enviando mensaje a pipe A\n");
            enviarMsg(fdA[1], "Post A\n");
            printf("Enviando mensaje a pipe B\n");
            enviarMsg(fdB[1], "Post B\n");
        }

        close(fdC[0]);
        close(fdX[0]);
        close(fdA[1]);
        close(fdB[1]);

        return 1;
    }

    // Lee de fdA y escribe en fdX
    // cierro escritura en A, B y C
    close(fdA[1]);
    close(fdB[1]);
    close(fdC[1]);
    // cierro lectura en B, C y X
    close(fdB[0]);
    close(fdC[0]);
    close(fdX[0]);

    for (int i = 0; i < (iter * 2); i++)
    {

        printf("Leyendo pipe A\n");
        recibirMsg(fdA[0], msg, 100);
        printf("A");
        printf("Enviando mensaje a pipe X\n");
        enviarMsg(fdX[1], "Post X\n");
    }

    close(fdA[0]);
    close(fdX[1]);

    printf("\nFin main!\n");

    return 0;
}

void enviarMsg(int fd, const char *msg)
{
    int bytes = write(fd, msg, strlen(msg) + 1);
    printf("Envie %d bytes\n", bytes);
}

void recibirMsg(int fd, char *dest, int size)
{
    int bytes = read(fd, dest, size);
    printf("Recibi %d bytes: %s\n");
}
