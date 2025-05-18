#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "sala.h"
#include "macros.h"
#include "hilos/retardo.h"

#ifndef CAPACIDAD
#define CAPACIDAD 20
#endif

pthread_mutex_t main_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_reservas = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_liberaciones = PTHREAD_COND_INITIALIZER;

int n, m;

void* mostrar_estado(void* arg) {
	
	while(n > 0 || m > 0) {
		estado_sala("\n\nSala en mitad del proceso");
		sleep(5);
	}
}

void* reservar(void* arg) {
	int id = *((int*) arg);
	
	for(int i = 0; i < 3; i++) {
		pthread_mutex_lock(&main_mutex);

		while (reserva_asiento(id) == -1) {
			if (m == 0) {
			        printf("No hay más hilos que liberan. Abortando hilo reservar %d.\n", id);
			        n--;
				pthread_mutex_unlock(&main_mutex);
				return NULL;
			}
			
			printf("Hilo %d esperando para reservar...\n", id);
			pthread_mutex_unlock(&main_mutex);
			pthread_cond_wait(&cond_reservas, &main_mutex);
			
		}

		pthread_cond_broadcast(&cond_liberaciones);
		pthread_mutex_unlock(&main_mutex);
		pausa_aleatoria(8);
	}

	pthread_mutex_lock(&main_mutex);
	n--;
	pthread_cond_broadcast(&cond_liberaciones);
	pthread_mutex_unlock(&main_mutex);
	
	printf("Hilo reserva %d ha terminado sus tareas.\n", id);
}


void* liberar(void* arg) {
	int id = *((int*) arg);
	
	for(int i = 0; i < 3; i++) {
	        pthread_mutex_lock(&main_mutex);
	        
		while (libera_cualquiera() == -1) {
			if(n == 0) {
			        printf("No hay más hilos que reservan. Abortando hilo liberar %d.\n", id);
			        m--;
				pthread_mutex_unlock(&main_mutex);
				return NULL;
			}
			
			printf("Hilo %d esperando para liberar...\n", id);
			pthread_mutex_unlock(&main_mutex);
			
			pthread_cond_wait(&cond_liberaciones, &main_mutex);
		}

		pthread_cond_broadcast(&cond_reservas);
		pthread_mutex_unlock(&main_mutex);
		pausa_aleatoria(8);
	}
	pthread_mutex_lock(&main_mutex);
	m--;
	pthread_cond_broadcast(&cond_reservas);
	pthread_mutex_unlock(&main_mutex);
	printf("Hilo libera %d ha terminado sus tareas.\n", id);
}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		fprintf(stderr, "Número de argumentos incorrecto. Introduzca \"multihilos n m\"\n");
		exit(1);
	}

	int hilos_reserva = atoi(argv[1]);
	int hilos_libera = atoi(argv[2]);

	n = hilos_reserva;
	m = hilos_libera;
	
	if (hilos_reserva <= 0 || hilos_libera <= 0) {
		fprintf(stderr, "Los argumentos 2/3 deben ser enteros positivos. Introduzca \"multihilos n m\"\n");
		exit(1);
	}
	
	pthread_t reserva[hilos_reserva], libera[hilos_libera], estado;
	int* ids_reserva = malloc(hilos_reserva * sizeof(int));
	int* ids_libera = malloc(hilos_reserva * sizeof(int));

	crea_sala(CAPACIDAD);
	
	for (int i = 0; i < hilos_reserva; i++) {
		*(ids_reserva + i) = i + 1;
		if (pthread_create(&reserva[i], NULL, reservar, &ids_reserva[i]) != 0) {
			elimina_sala();
			free(ids_reserva);
			free(ids_libera);
			perror("Error en la creación de los hilos");
			exit(1);
		}
	}
	
	for (int i = 0; i < hilos_libera; i++) {
		*(ids_libera + i) = i + 1;
		if (pthread_create(&libera[i], NULL, liberar, &ids_libera[i]) != 0) {
			elimina_sala();
			free(ids_reserva);
			free(ids_libera);
			perror("Error en la creación de los hilos");
			exit(1);
		}
	}

	if (pthread_create(&estado, NULL, mostrar_estado, NULL) != 0) {
		elimina_sala();
		free(ids_reserva);
		free(ids_libera);
		perror("Error en la creación de los hilos");
		exit(1);
	}
	
	for (int i = 0; i < hilos_reserva; i++) {
		pthread_join(reserva[i], NULL);
	}
	for (int i = 0; i < hilos_libera; i++) {
		pthread_join(libera[i], NULL);
	}

	estado_sala("\n\nEstado final de la sala");

	elimina_sala();
	pthread_join(estado, NULL);

	free(ids_reserva);
	free(ids_libera);
	exit(0);
}


