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

int ejecutando = 0;

void* mostrar_estado(void* arg) {

	while (ejecutando) {
		estado_sala("\n\nSala en mitad del proceso");
		sleep(3);
	}
}

void* reserva_anula(void* arg) {
	int id = *((int*) arg);
	
	for(int i = 0; i < 3; i++) {
		if (reserva_asiento(id) == -1) {
		        // Hemos entendido que por "terminará su ejecución sin ningún efecto sobre la sala", significa
		        // que el hilo deshace los cambios hechos, dejando la sala como estaba antes de la transacción.
		        // En caso de referirse a que el hilo debería abortar sin deshacer sus cambios, bastaría con
		        // eliminar el bucle for, y únicamente devolver NULL para abortar el hilo.
			for (int j = 0; j < i; j++) {
				levantarse(id);
			}
			return NULL;
		}
		pausa_aleatoria(2);
	}

	for(int i = 0; i < 3; i++) {
		if(levantarse(id) == -1){
			printf("Hilo %d ha intentado liberar y no ha podido, abortando hilo %d.\n", id, id);
			return NULL; 
		}
		pausa_aleatoria(2);
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
	ejecutando = 1;
	
	for (int i = 0; i < n_hilos; i++) {
		*(ids + i) = i + 1;
		if (pthread_create(&hilos[i], NULL, reserva_anula, ids+i) != 0) {
			CHECK_HILOS("Error en la creación de los hilos");
		}
	}
	
	if (pthread_create(&estado, NULL, mostrar_estado, NULL) != 0) {
		CHECK_HILOS("Error en la creación del hilo de estado");
	}
	
	for (int i = 0; i < n_hilos; i++) {
		pthread_join(hilos[i], NULL);
	}
	
	ejecutando = 0;
	pthread_join(estado, NULL);
	estado_sala("Estado final de la sala");
	elimina_sala();
	free(ids);
}
