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


void* mostrar_estado(void* arg) {
	
	while(capacidad_sala() > 0) {
		estado_sala("\n\nSala en mitad del proceso");
		sleep(3);
	}
}

void* reserva_anula(void* arg) {
	int id = *((int*) arg);
	for(int i = 0; i < 3; i++) {
		reserva_asiento(id);
		pausa_aleatoria(5);
	}

	for(int i = 0; i < 3; i++) {
		levantarse(id);
		pausa_aleatoria(5);
	}
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		fprintf(stderr, "Número de argumentos incorrecto. Introduzca \"multihilos n\"\n");
		exit(1);
	}

	int n_hilos = atoi(argv[1]);
	
	if (n_hilos <= 0) {
		fprintf(stderr, "El segundo argumento debe ser un entero positivo. Introduzca \"multihilos n\"\n");
		exit(1);
	}
	
	pthread_t hilos[n_hilos], estado;
	int* ids = (int*) malloc(n_hilos * sizeof(int));
	crea_sala(CAPACIDAD);
	
	for (int i = 0; i < n_hilos; i++) {
		*(ids + i) = i + 1;
		if (pthread_create(&hilos[i], NULL, reserva_anula, ids+i) != 0) {
			elimina_sala();
			perror("Error en la creación de los hilos");
			exit(1);
		}
	}
	
	if (pthread_create(&estado, NULL, mostrar_estado, NULL) != 0) {
		elimina_sala();
		perror("Error en la creación de los hilos");
		exit(1);
	}
	
	for (int i = 0; i < n_hilos; i++) {
		pthread_join(hilos[i], NULL);
	}
	estado_sala("Estado final de la sala");
	elimina_sala();
	pthread_join(estado, NULL);
	free(ids);
}
