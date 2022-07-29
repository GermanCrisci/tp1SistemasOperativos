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

int main(int argc, char *argv[])
{    
	// elimino la memoria compartida para reiniciar los semaforos

	shm_unlink("/shm");
	
	
    if (argv[1] == NULL)
    {
        printf("Por favor ingrese el numuero de iteraciones como argumento\n");
        return -1;
    }
    
	// creo la memoria compartida y la mapeo con la estructura semaforos
	
	int fd = shm_open("/shm", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {printf("Error shm_open\n") ; exit(-1) ;} 
	
	if (ftruncate(fd, sizeof(struct semaforos)) == -1) {printf("Error ftruncate\n") ; exit(-1) ;}
	
	struct semaforos *shm = mmap(NULL, sizeof(struct semaforos), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shm == MAP_FAILED) {printf("Error mmap\n") ; exit(-1) ;}
		
	if (sem_init(&shm->semA, 1, 1) == -1) {printf("Error al inicializar semA\n") ; exit(-1) ;}
	if (sem_init(&shm->semB, 1, 1) == -1) {printf("Error al inicializar semA\n") ; exit(-1) ;}
	if (sem_init(&shm->semC, 1, 0) == -1) {printf("Error al inicializar semA\n") ; exit(-1) ;}
	if (sem_init(&shm->semX, 1, 0) == -1) {printf("Error al inicializar semA\n") ; exit(-1) ;}

    int valSemA;
    int valSemB;
    int valSemC;
    int valSemX;

    sem_getvalue(&shm->semA, &valSemA);
    sem_getvalue(&shm->semB, &valSemB);
    sem_getvalue(&shm->semC, &valSemC);
    sem_getvalue(&shm->semX, &valSemX);

    printf("Valor Semaforo A = %d \n", valSemA);
    printf("Valor Semaforo B = %d \n", valSemB);
    printf("Valor Semaforo C = %d \n", valSemC);
    printf("Valor Semaforo X = %d \n", valSemX);

    
    int iter = atoi(argv[1]);
    printf("Se iterara %d veces\n", iter);
    
    // elimino la memoria compartida en caso de que exista
    
    int shmidaux = shmget(0xA, 0, 0);
    shmctl(shmidaux, IPC_RMID, NULL);

	// me guardo en memoria compartida el numero de iteraciones

    int shmid = shmget(0xA, sizeof(int), IPC_CREAT | IPC_EXCL | 0700);
    if (shmid < 0)
    {
		printf("Hubo un error al guardar el numero de iteraciones en memoria compartida\n");
    }
    
    printf("shmid %d \n", shmid);
    int *dir = (int *) shmat(shmid, 0, 0);
    *dir = iter;
    shmdt(dir);
}

