#include <pthread.h>
#include <unistd.h> // funciones unix (sleep,read(), write(), etc)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//

int matriz[3][4]; // ventas de 3 columnas con 4 filas
int totalgral = 0.0;
int totalColumnas[3] = {-1, -1, -1};
int sumadas = 0;
int cargadas[3] = {0, 0, 0}; // si esta cargada la columna
int columnas[3] = {0, 1, 2};

// prototipo de funciones
void *sumaColumnas(void *);
void *cargaMatriz(void *);
void *sumaGeneral(void *);

// mutex y variables de condicion para la sumatoria de cada columna
pthread_mutex_t cargada_mutex[4];
pthread_cond_t cargada_cv[4];
// mutex y variables de condicion para operacion de sumatoria general
pthread_mutex_t sumadas_mutex;
pthread_cond_t sumadas_cv;

int main(int argc, char *argv[])
{
    int i;

    pthread_t hilo[4];
    pthread_t hiloCarga, hiloSumaGeneral;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for (i = 0; i < 3; i++)
    {
        pthread_mutex_init(&cargada_mutex[i], NULL);
        pthread_cond_init(&cargada_cv[i], NULL);
    }

    printf("main(): creando hilo para suma de totales generales\n");
    pthread_create(&hiloSumaGeneral, NULL, sumaGeneral, NULL);
    
    printf("main(): creando hilos para suma de totales por columna\n");
    for (i = 0; i < 3; i++)
    {
        printf("main(): creando hilo sumaColumnas para columna %d\n", i);
        pthread_create(&hilo[i], &attr, sumaColumnas, &columnas[i]);
    }

    printf("main(): creando hilo para carga de la matriz\n");
    pthread_create(&hiloCarga, NULL, cargaMatriz, NULL);

    // libero recurso que ya no necesito
    pthread_attr_destroy(&attr);

    // hago joins
    for (i = 0; i < 3; i++) {
        printf("main(): haciendo join con thread total por columna %d\n", i);
        pthread_join(hilo[i], NULL);
        printf("main(): termino join con thread total por columna %d\n", i);
    }

    printf("main(): haciendo join con carga de matriz\n");
    pthread_join(hiloCarga, NULL);
    printf("main(): termino join con carga de matriz\n");
    
    
    printf("main(): haciendo join con carga de totales generales\n");
    pthread_join(hiloSumaGeneral, NULL);
    printf("main(): termino join con carga de totales generales\n");
    

    printf("main(): imprimo filas/columnas y totales\n");
    
    printf("----- MATRIZ -----\n");
    for (i = 0; i < 4; i++)
    {
        printf("%d\t%d\t%d\n", matriz[0][i], matriz[1][i], matriz[2][i]);
    }

    printf("----- TOTALES ----\n");
    printf("%d\t%d\t%d\n", totalColumnas[0], totalColumnas[1], totalColumnas[2]);
    
    
    printf("----- TOTAL ----\n");
    printf("\t%d\t\n", totalgral);

    printf("fin main():\n");
    pthread_exit(NULL);
}

void *cargaMatriz(void *p)
{
    int c, f;
    printf("cargaMatriz():inicio\n");
    // para cada columna ...
    for (c = 0; c < 3; c++)
    {
        pthread_mutex_lock(&cargada_mutex[c]);
        // para cada fila ...
        for (f = 0; f < 4; f++) {
            matriz[c][f] = ((double)random()) / 100000000;
        }
        cargadas[c] = 1;
        pthread_cond_signal(&cargada_cv[c]);
        pthread_mutex_unlock(&cargada_mutex[c]);
        sleep(1);
    }
    printf("cargaMatriz():fin!\n");
    pthread_exit(NULL);
}

void *sumaColumnas(void *hilo)
{
    int tid;
    tid = *((int *) hilo);
    printf("sumaColumnas():hilo %d columna %d\n", tid, tid);
    pthread_mutex_lock(&cargada_mutex[tid]);
    if (!cargadas[tid])
    {
        printf("\tsumaColumnas():hilo %d columna %d esperando condicion!\n", tid, tid);
        pthread_cond_wait(&cargada_cv[tid], &cargada_mutex[tid]);
    }
    printf("\tsumaColumnas():sumo! hilo %d columna %d\n", tid, tid);
    int d;
    totalColumnas[tid] = 0;
    // printf("\tsumaColumnas():hilo %d columna %d valor inical suma columna = %d\n", tid, tid, totalColumnas[tid]);
    for (d = 0; d < 4; d++) {
        // printf("\tsumaColumnas():hilo %d columna %d valor columna[%d][%d] = %d\n", tid, tid, tid, d, matriz[tid][d]);
        totalColumnas[tid] += matriz[tid][d];
        // printf("\tsumaColumnas():hilo %d columna %d valor despues de suma columna = %d\n", tid, tid, totalColumnas[tid]);
    }
    sumadas++;
    // si ya sume todo, se#alo cv sumadas
    if (sumadas >= 3)
        pthread_cond_signal(&sumadas_cv);
    printf("sumaColumnas():fin! hilo %d sumatoria de columna %d  %d\n", tid, tid, totalColumnas[tid]);
    pthread_mutex_unlock(&cargada_mutex[tid]);
    pthread_exit(NULL);
}

void *sumaGeneral(void *threadid)
{
    printf("sumaGeneral():inicio!\n");
    pthread_mutex_lock(&sumadas_mutex);
    if (sumadas < 3)
    {
        printf("\tsumaGeneral():esperando condicion de columnas sumadas\n");
        pthread_cond_wait(&sumadas_cv, &sumadas_mutex);
    }
    printf("\tsumaGeneral():sumo valores totales de columnas\n");
    totalgral = 0;
    int col;
    for (col = 0; col < 3; col++) {
        totalgral += totalColumnas[col];
    }
    printf("sumaGeneral():fin! sumatoria de columnas %d\n", totalgral);
    pthread_mutex_unlock(&sumadas_mutex);
    pthread_exit(NULL);
}