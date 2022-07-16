#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <dirent.h>

char *leerLinea();
char **parsearLinea(char *);
void ejecutar(char *, char **);
int esComando(char **);
void ejecutarComando(char *, char **);

int salir = 0;
int main()
{
    while (!salir)
    {
        printf(">> ");
        // lee la linea del usuario
        char *linea = leerLinea();
        // separa la linea en tokens
        char **tokens = parsearLinea(linea);
        if (tokens[0] != NULL)
        {
            ejecutar(linea, tokens);
        }
        free(tokens);
        free(linea);
    }
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
    // me guardo los chars por los que voy a separar los tokens
    char *delimitadores = " \t\r\n";
    // tomo los tokens
    char *token = strtok(linea, delimitadores);
    while (token != NULL)
    {
        tokens[largo] = token;
        largo++;
        // si me paso del espacio, lo amplio
        // con ralloc
        if (largo >= capacidad)
        {
            capacidad = (int)capacidad * 2;
            tokens = realloc(tokens, capacidad * sizeof(char *));
        }

        // tomo el siguiente token
        token = strtok(NULL, delimitadores);
    }
    // agrego el argumento nulo al final
    tokens[largo] = NULL;
    return tokens;
}

void ejecutar(char *linea, char **args)
{
    int foreground = 0;
    int binario = 0;
    // cuento los argumentos.
    int argc = 0;
    while (args[argc] != NULL)
        argc++;

    // me fijo si es un programa o un comando
    // si es un comando verifico si es uno valido.
    if (esComando(args))
    {
        ejecutarComando(linea, args);
    }
    else
    {
        // me fijo si es fore o back.
        // printf("Ultumo argumento; \"%s\"\n", args[argc - 1]);
        if (!(args[argc - 1] == "&"))
            foreground = 1;

        pid_t pid = fork();

        if (pid == 0) // hijo
        {
            execvp(args[0], args);
            printf("Error Exee!\n");
            exit(1);
        }
        else if (pid > 0)
        {
            int estado;
            // printf("Esperando al hijo pid %d\n", pid);
            waitpid(pid, &estado, 0);
            // printf("Termino hijo pid %d con estado %d\n", pid, estado / 256);
        }
        else
        {
            printf("Error fork");
        }
    }
}

int esComando(char **args)
{
    char *comandos[2] = {"lista", "salir"};
    int esComando = 0;
    printf("Comparando comandos!\n");
    int i = 0;
    for (; i < 2; i++)
    {
        // Nota: strncmp devuelve 0 si son iguales
        if (!strncmp(comandos[i], args[0], strlen(comandos[i])))
        {
            esComando++;
            break;
        }
    }

    return esComando;
}

void ejecutarComando(char *linea, char **args)
{
    if (!strncmp("salir", args[0], 5))
    {
        printf("Saliendo\n");
        salir = 1;
    }
    if (!strncmp("lista", args[0], 5))
    {
        printf("Mostrando directorio\n");
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if(d){
            while((dir = readdir(d)) != NULL){
                printf("%s\n", dir->d_name);
            }
            closedir(d);
        }

    }
}