#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "sala.h"
#include "hilos/retardo.h"

#ifndef CAPACIDAD
#define CAPACIDAD 10
#endif

// reto 3: poner pausas largas para que se vea, mandar vídeo por sharepoint

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_reservas = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_liberaciones = PTHREAD_COND_INITIALIZER;

void* mostrar_estado(void* arg) {
	
	while(capacidad_sala() > 0) {
		pthread_mutex_lock(&mutex);
		estado_sala("\n\nSala en mitad del proceso");
		pthread_mutex_unlock(&mutex);
		sleep(5);
	}
}

void* reservar(void* arg) {
	int id = *((int*) arg);
	for(int i = 0; i < 3; i++) {
		pthread_mutex_lock(&mutex);

		while (asientos_libres() == 0) {
			printf("%d esperando para reservar...\n", id);
			pthread_cond_wait(&cond_reservas, &mutex);
		}

		reserva_asiento(id);
		pthread_cond_signal(&cond_liberaciones);
		pthread_mutex_unlock(&mutex);
		pausa_aleatoria(5);
	}
}

void* liberar(void* arg) {
	int id = *((int*) arg);
	for(int i = 0; i < 3; i++) {
		pthread_mutex_lock(&mutex);

		while (asientos_ocupados() == 0) {
			printf("%d esperando para liberar...\n", id);
			pthread_cond_wait(&cond_liberaciones, &mutex);
		}

		libera_cualquiera();
		pthread_cond_signal(&cond_reservas);
		pthread_mutex_unlock(&mutex);
		pausa_aleatoria(5);
	}
}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		fprintf(stderr, "Número de argumentos incorrecto. Introduzca \"multihilos n m\"\n");
		exit(1);
	}

	int hilos_reserva = atoi(argv[1]);
	int hilos_libera = atoi(argv[2]);
	
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
