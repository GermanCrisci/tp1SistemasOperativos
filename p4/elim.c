#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[])
{
    sem_unlink("/semA");
    sem_unlink("/semB");
    sem_unlink("/semC");
    sem_unlink("/semX");
    // borro de memoria compartida 
    // el numero de iteraciones
    int shmidNum = shmget(0xa, 0, 0);
    shmctl(shmidNum, IPC_RMID, NULL);
}