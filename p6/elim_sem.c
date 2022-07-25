#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>

int main(int argc, char *argv[])
{
    int semid = semget(0xa, 0, 0);
    semctl(semid, IPC_RMID, 0);
    int shmid = shmget(0xa, 0, 0);
    shmctl(shmid, IPC_RMID, 0);

}