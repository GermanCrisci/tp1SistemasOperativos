#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/shm.h>

int main(int argc, char *argv[])
{
    sem_unlink("/semA");
    sem_unlink("/semB");
    sem_unlink("/semC");
    sem_unlink("/semX");

    shm_unlink(0xd);

}