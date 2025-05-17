#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "sala.h"
#include "macros.h"
#include "hilos/retardo.h"

#ifndef CAPACIDAD
#define CAPACIDAD 10
#endif

extern pthread_mutex_t mutex;
pthread_mutex_t end_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_reservas = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_liberaciones = PTHREAD_COND_INITIALIZER;

int n_max, n, m;

void* mostrar_estado(void* arg) {
	
	while(capacidad_sala() > 0) {
		estado_sala("\n\nSala en mitad del proceso");
		sleep(5);
	}
}

void* reservar(void* arg) {
	int id = *((int*) arg);
	for(int i = 0; i < 3; i++) {

		while (reserva_asiento(id) == -1) {
			pthread_mutex_lock(&end_mutex);
			if (m == 0) {
				pthread_mutex_unlock(&end_mutex);
				return NULL;
			}
			pthread_mutex_unlock(&end_mutex); //deberíamos bajar el mutex

			printf("%d esperando para reservar...\n", id);
			pthread_cond_wait(&cond_reservas, &mutex);
		}

                puts("Se ha reservado...");
		pthread_cond_broadcast(&cond_liberaciones);
		pausa_aleatoria(5);
	}

	pthread_mutex_lock(&end_mutex);
	n--;
	pthread_cond_broadcast(&cond_liberaciones);
	pthread_mutex_unlock(&end_mutex);
}


void* liberar(void* arg) {
	int id = *((int*) arg);
	for(int i = 0; i < 3; i++) {

		while (libera_cualquiera() == -1) {
			pthread_mutex_lock(&end_mutex);
			if(n == 0) {
				pthread_mutex_unlock(&end_mutex);
				return NULL;
			}
			pthread_mutex_unlock(&end_mutex);
			printf("%d esperando para liberar...\n", id); //puede darse el caso de que, en lo que se printea la notificación del ultimo que liber no llegue a este hio y se queda esperando eternamente, solucion quitar el printf, o printear con dentro de la seccion crítica para que nadie pueda modificar en lo que tu estaás printando
			pthread_cond_wait(&cond_liberaciones, &mutex);
		}

		pthread_cond_broadcast(&cond_reservas);
		pausa_aleatoria(5);
	}
	pthread_mutex_lock(&end_mutex);
	m--;
	pthread_cond_broadcast(&cond_reservas);
	pthread_mutex_unlock(&end_mutex);
}


int main(int argc, char* argv[]) {

	if (argc != 4) {
		fprintf(stderr, "Número de argumentos incorrecto. Introduzca \"multihilos maximo_de_hilos_por_grupo nhilos_reservas nhilos_liberaciones\"\n");
		exit(1);
	}

	n_max = atoi(argv[1]);
	int hilos_reserva = atoi(argv[2]);
	int hilos_libera = atoi(argv[3]);

	n = hilos_reserva;
	m = hilos_libera;
	
	if (hilos_reserva <= 0 || hilos_libera <= 0 || n_max <= 0) {
		fprintf(stderr, "Los argumentos deben ser enteros positivos. Introduzca \"multihilos maximo_de_hilos_por_grupo nhilos_reservas nhilos_liberaciones\"\n");
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

void join_reserva
