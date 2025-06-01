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
	while (1) {
		pthread_mutex_lock(&main_mutex);
		if (n > 0 && m > 0) {
			pthread_mutex_unlock(&main_mutex);
			break;
		}
		pthread_mutex_unlock(&main_mutex);
		estado_sala("\n\nSala en mitad del proceso");
		sleep(3);
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

	if (argc != 4) {
		fprintf(stderr, "Número de argumentos incorrecto. Introduzca \"multihilos n res lib\"\n");
		exit(1);
	}

	int hilos_libera= atoi(argv[3]);
	int hilos_reserva = atoi(argv[2]);
	int N_grupo = atoi(argv[1]);
	
	if (hilos_reserva <= 0 || hilos_libera <= 0 || N_grupo <= 0) {
		fprintf(stderr, "Los argumentos 2/3/4 deben ser enteros positivos. Introduzca \"multihilos n res lib\"\n");
		exit(1);
	}

	pthread_t reserva[hilos_reserva], libera[hilos_libera], estado;
	int* ids_reserva = malloc(hilos_reserva * sizeof(int));
	int* ids_libera = malloc(hilos_libera * sizeof(int));

	crea_sala(CAPACIDAD);
	
	if (pthread_create(&estado, NULL, mostrar_estado, NULL) != 0) {
		ERROR_HILOS("Error en la creación del hilo mostrar estado");
	}

	int ni = hilos_reserva;
	int mi = hilos_libera;

	int res, lib;

	while (ni > 0 || mi > 0) {

		pthread_mutex_lock(&main_mutex);
		res = 0;
		lib = 0;

		for (int i = 0; i < N_grupo && ni > 0; i++) {
			*(ids_reserva + hilos_reserva - ni) = hilos_reserva - ni + 1;
			if (pthread_create(&reserva[hilos_reserva-ni], NULL, reservar, &ids_reserva[hilos_reserva-ni]) != 0) {
				ERROR_HILOS("Error en la creación de los hilos (reserva)");
			}
			res++;
			n++;
			ni--;
		}
		
		for (int i = 0; i < N_grupo && mi > 0; i++) {
			*(ids_libera + hilos_libera - mi) = hilos_libera - mi + 1;
			if (pthread_create(&libera[hilos_libera-mi], NULL, liberar, &ids_libera[hilos_libera-mi]) != 0) {
				ERROR_HILOS("Error en la creación de los hilos (libera)");
			}
			lib++;
			m++;
			mi--;
		}
		pthread_mutex_unlock(&main_mutex);
		
		for (int i = res; i > 0; i--) {
			pthread_join(reserva[hilos_reserva-ni-i], NULL);
		}
		for (int i = lib; i > 0; i--) {
			pthread_join(libera[hilos_libera-mi-i], NULL);
		}
		puts("Pausa...");
		sleep(10);
	}
	
	
	pthread_join(estado, NULL);
	estado_sala("\n\nEstado final de la sala");
	elimina_sala();
	free(ids_reserva);
	free(ids_libera);
	exit(0);

} 

