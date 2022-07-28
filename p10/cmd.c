#define _GNU_SOURCE // habilita funciones como strcasestr() etc

// para habilitar un conjunto de funciones que necesitan algunas de las funciones implementadas

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <time.h>
#include <wait.h>

void printError(char *);
int extraer(char *destino, int largo, const char *tag, char *buffer);
int esHexa(char c);
char charHexa(char *p);
int obtenercgi(char *destino, int largo);
void tagInputText(const char *label, const char *id, const char *value);
void tagInput(const char *label, const char *id, const char *tipo, const char *value);
long long timestamp();
void logmsg(const char *archivo, const char *programa, const char *mensaje);
void parsearLinea(char *linea);
int comandoPermitido();

char cmdargs[2048], args[1024], cmd[1024], msg[300];
char cmdPermitidos[2][1024] = {"ps", "ls"};
char *argumentos[100];
char *argumentosCombinado[101];

int main(int argc, char *argv[])
{
    pid_t pid;

    memset(cmdargs, 0, 2048);
    int rc = obtenercgi(cmdargs, 2048);
    printf("obtenercgi retorna %d metodo [%s]\n", rc, getenv("REQUEST_METHOD"));
    extraer(cmd, 1024, "cmd", cmdargs);
    printf("cmd=[%s]\n", cmd);
    extraer(args, 1024, "args", cmdargs);
    printf("args=[%s]\n", args);
    snprintf(msg, 300, "obtenercgi retorna %d metodo [%s]", rc, getenv("REQUEST_METHOD"));
    logmsg("login.txt", argv[0], msg);
    parsearLinea(args);

    if (comandoPermitido()) {
        argumentosCombinado[0] = cmd;
        for (size_t i = 0; i < 100; i++)
        {
            if (argumentos[i] == NULL)
            {
                argumentosCombinado[i + 1] = NULL;
            }
            argumentosCombinado[i + 1] = argumentos[i];
        }

        if ((pid = fork()) == -1)
            printError("fork");

        if (pid == 0)
        {
            execvp(cmd, (char **)argumentosCombinado);
            printError("execvp");
        }
        else
        {
            wait(0); // espero a que termine el hijo
        }          
    } else {
        printf("ERROR: Intento correr un comando no permitido\n");
    }
    return 1;
}

int comandoPermitido()
{
    int c = 0;
    while(c < (sizeof(cmdPermitidos)/sizeof(cmdPermitidos[0]))){
        if (strcmp(cmd,cmdPermitidos[c])) return 1;
        c++;
    }
    return 0;
}

void parsearLinea(char *linea)
{
    // printf("Linea argumentos %s\n", linea);
    int largo = 0;
    // me guardo los chars por los que voy a separar los tokens
    char *delimitadores = " ";
    // tomo los tokens
    char *token = strtok(linea, delimitadores);
    while (token != NULL)
    {
        argumentos[largo] = token;
        // printf("Token: %s\n", token);
        largo++;
        // tomo el siguiente token
        token = strtok(NULL, delimitadores);
    }
    // agrego el argumento nulo al final
    argumentos[largo] = NULL;
}

void printError(char *str)
{
    printf("Hubo un error en seccion %s", str);
}

void tagInput(const char *label, const char *id, const char *tipo, const char *value)
{
    printf("<td><label for=\"%s\">%s</label></td>", id, label);
    printf("<td><input type=\"%s\" id=\"%s\" name=\"%s\" value=\"%s\"></td>\n", tipo, id, id, value);
}

void tagInputText(const char *label, const char *id, const char *value)
{
    tagInput(label, id, "text", value);
}

/**
 * Ojo con los espacios intermedios, se reemplazan por +
 * Ojo con mayusculas y minusculas
 * buffer contiene:   variable1=valor1&variable2=valor2....
 *
 * Toma buffer, busca tag y copia valor en destino
 * destino tiene que tener espacio suficiente
 * retorna verdad si pudo extraer algo, sino retorna falso
 */
int extraer(char *destino, int largo, const char *tag, char *buffer)
{
    char *p = strcasestr(buffer, tag), *out = destino;
    int n = 0;
    if (p)
    {
        p += strlen(tag) + 1;
        if (*p)
        {
            while (*p && *p != '&' && n < largo)
            {
                if (*p == '\n' || *p == '\r' || *p == '\t')
                {
                    p++;
                    continue;
                }
                if (*p == '+')
                {
                    *out = ' ';
                }
                else
                {
                    // posible caracter codificado %00 .. %FF
                    if (*p == '%' && esHexa(*(p + 1)) && esHexa(*(p + 2)))
                    {
                        *out = charHexa(p);
                        p += 2;
                    }
                    else
                    {
                        *out = *p;
                    }
                }
                out++;
                p++;
                n++;
            }
            *out = '\0';
            return 1;
        }
    }
    return 0;
}

/**
 * Devuelve 1 (verdad) si c es un digito hexadecimal
 * Sino devuelve 0 (falso)
 */
int esHexa(char c)
{
    char c2 = (char)toupper(c);
    if (c2 >= '0' && c2 <= '9')
        return 1;
    if (c2 >= 'A' && c2 <= 'F')
        return 1;
    return 0;
}
/**
 * p apunta a "%2A..." por ejemplo, esta funcion devuelve
 * el caracter ascii del valor hexa 2A
 */
char charHexa(char *p)
{
    char hexa[3];
    hexa[0] = *(p + 1);
    hexa[1] = *(p + 2);
    hexa[2] = '\0';
    return (char)strtol(hexa, NULL, 16);
}

/**
 * Esta funcion obtiene el string cgi cargado por el servidor
 * web para luego ser procesado
 * Puede tratarse de una llamada de tipo GET o POST
 * GET el string cgi se encuentra en variable QUERY_STRING
 * POST el string se ingresa por teclado
 * Esta funcion sirve para ambos casos
 * Retorna 1 si todo Ok
 * Retorna 0 si hubo error
 */
int obtenercgi(char *destino, int largo)
{
    if (getenv("QUERY_STRING") == NULL)
        return 0;
    if (getenv("REQUEST_METHOD") == NULL)
        return 0;
    if (strcasecmp(getenv("REQUEST_METHOD"), "GET") == 0)
    { // es GET
        if (strlen(getenv("QUERY_STRING")) > largo)
        {
            // el contenido de query_string supera a largo!
            return 0;
        }
        strcpy(destino, getenv("QUERY_STRING"));
    }
    else
    { // es POST
        int lei = read(STDIN_FILENO, destino, largo - 1);
        if (lei >= 0)
            destino[lei] = '\0';
    }
    return 1;
}

/* Obtener la cantidad de milisegundos desde el inicio del tiempo! */
long long timestamp()
{
    struct timeval te;
    gettimeofday(&te, NULL);                                         // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
// graba mensaje en archivo en modo append (al final del archivo), agrega intro luego del mensaje
void logmsg(const char *archivo, const char *programa, const char *mensaje)
{
    FILE *fl = fopen(archivo, "a");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(fl, "%d-%02d-%02d %02d:%02d:%02d:%s:%s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, programa, mensaje);
    fclose(fl);
}