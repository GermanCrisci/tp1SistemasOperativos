/*
 * p8.c
 * 
 * Copyright 2022 Fede <fede@debian>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * gcc -Wall -o p8 p8.c -pthread   
 */


#include <pthread.h>  
#include <unistd.h>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double matriz[4][3]; 
double subtotales[3] = {-1,-1,-1};
double totalMatriz = 0.0;
int columnas[3] = {0,1,2};
int sumadas = 0;

void *cargoMatriz(void *);  
void *sumoColumna(void *);  
void *sumoTotal(void *);   //suma toda la matriz

//defino otro mutex para controlar que no se haga la suma antes de cargar la matriz
pthread_mutex_t m1,m2;
pthread_cond_t v;



int main(int argc, char **argv)
{
	int i;
	pthread_t hilo[3];  //un hilo para sumar cada columna
	pthread_t carga,total;
	
	pthread_mutex_init(&m1,NULL);
	pthread_mutex_init(&m2,NULL);
	pthread_cond_init(&v,NULL);
	
	pthread_mutex_lock(&m2);
	
	for (i=0;i<3;i++) pthread_create(&hilo[i],NULL,sumoColumna,&columnas[i]);
	pthread_create(&carga,NULL,cargoMatriz,NULL);
	pthread_create(&total,NULL,sumoTotal,NULL);
	
	for(i=0;i<3;i++) pthread_join(hilo[i],NULL);
	pthread_join(carga,NULL);
	pthread_join(total,NULL);
	
	
	for(i=0;i<3;i++) {
		printf("%2.0f\t%2.0f\t%2.0f\n",matriz[i][0],matriz[i][1],matriz[i][2]);
	}
	
	printf("----SUBTOTALES----\n");
	printf("%2.0f\t%2.0f\t%2.0f\n",subtotales[0],subtotales[1],subtotales[2]);
	
	printf("-------TOTAL------\n");
	printf("\t%3.0f\n",totalMatriz);
		
	
	return 0;
}


void *cargoMatriz(void *t) {
	
	int f,c;
	pthread_mutex_lock(&m1);
	
	for(f=0;f<4;f++) {
		for(c=0;c<3;c++) {
			matriz[f][c] = ((double) random())/100000000.0;
		}
	}
			
	pthread_mutex_unlock(&m1);		
	pthread_mutex_unlock(&m2);
	
	pthread_exit(NULL);
}	

void *sumoColumna (void *r) {
	int col = *((int *) r);
	int t;
	
	pthread_mutex_lock(&m2);
	pthread_mutex_lock(&m1);
		
	for(t=0;t<3;t++) {
		subtotales[col] += matriz[t][col];
	}
	subtotales[col] += 1;  //nose xq me queda uno menos de lo que deberia
	sumadas++;
	
	if (sumadas >= 3) {
		pthread_cond_signal(&v);
	}
	
	pthread_mutex_unlock(&m1);
	pthread_mutex_unlock(&m2);	 	 
	
	pthread_exit(NULL);
}

void *sumoTotal (void *t) {
	
	int s;
	
	pthread_mutex_lock(&m1);
	
	if (sumadas < 3) {
		pthread_cond_wait(&v,&m1);
	}
	
	for (s=0;s<3;s++) {
		totalMatriz += subtotales[s];
	}
	
	pthread_mutex_unlock(&m1);
	
	pthread_exit(NULL);	
}		
	
	
		
