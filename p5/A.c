#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct semaforos
{
	sem_t semA;
	sem_t semB;
	sem_t semC;
	sem_t semX;
};

int main(void)
{    

	// abro la memoria compartida de semaforos para poder acceder en este proceso
	// se repite para B y C

	int fd = shm_open("/shm", O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {printf("Error shm_open\n") ; exit(-1) ;} 

    struct semaforos *shm = mmap(NULL, sizeof(struct semaforos), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shm == MAP_FAILED) {printf("Error mmap\n") ; exit(-1) ;}
	
	// tomo la cantidad de iteraciones que guarde en la memoria compartida
	// se repite en B y C
	
	int shmid = shmget(0xA, 0, 0);
    int *iter = (int *) shmat(shmid, 0, 0); 
	
	for (int i = 0; i < (*iter)*2; i++)
	{
		sem_wait(&shm->semA);
            printf("A\n");
            sleep(1);
        sem_post(&shm->semX);
	}
	
	shmdt(iter);
}
