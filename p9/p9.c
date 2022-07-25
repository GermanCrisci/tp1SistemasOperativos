#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <dirent.h>

void f_sigchld(int);

char *leerLinea();
char **parsearLinea(char *);

char msg[4096];
int fd[2];

int main(int argc, char **argv) {
	pid_t pid;
	int pip = pipe(fd);
	
	if (pip == -1) {
		printf("Error pipe\n");
}
	//pido el comando y utilizo las funciones hechas en el punto 1
	printf("Ingrese comando:\n");
	char *linea = leerLinea();
	char **tokens = parsearLinea(linea);

	pid = fork();

	if (pid == -1) {
		printf("Error fork\n");
}

	if(pid == 0) {
		//redirijo la salida del hijo al pipe
		dup2 (fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		execvp(tokens[0], tokens);
		printf("Fallo exevp");

} else {
	signal(SIGCHLD, (void*) f_sigchld);
	close(fd[1]);
	//el padre espera la señal sigchld del hijo
	pause();
	

}
	return 0;
}

char *leerLinea()
{
    char *linea = NULL;
    size_t largoBuffer = 0;
    getline(&linea, &largoBuffer, stdin);
    return linea;
}

char **parsearLinea(char *linea)
{
    int largo = 0;
    int capacidad = 16;
    char **tokens = malloc(capacidad * sizeof(char *));
    char *delimitadores = " \t\r\n";
    char *token = strtok(linea, delimitadores);
    while (token != NULL)
    {
        tokens[largo] = token;
        largo++;
        if (largo >= capacidad)
        {
            capacidad = (int)capacidad * 2;
            tokens = realloc(tokens, capacidad * sizeof(char *));
        }

        token = strtok(NULL, delimitadores);
    }
    tokens[largo] = NULL;
    return tokens;
}

void f_sigchld(int s){
    pid_t pid;
    int status;

    while ((pid = wait(&status)) > 0) {
        printf("Hijo con pid=%d termino\n", pid);
        int bytes = read(fd[0], msg, sizeof(msg));
        printf("leyendo del pipe %i bytes\n", bytes);
        printf("%s\n",msg);
        
    }
}
