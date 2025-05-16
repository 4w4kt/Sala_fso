//
// Created by aleja on 01/03/2025.
//

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>


#include "sala.h"
#include "macros.h"
#include <stdlib.h>

int* sala = NULL;
int cap_sala;
int ocupados;

/**
 * Reserva un asiento libre a una persona.
 * @param id_persona identificador de la persona que reserva el asiento
 * @return número de asiento si hay un asiento libre, -1 en cualquier otro caso
 */
int reserva_asiento(int id_persona) {
    pthread_mutex_lock(&mutex);
    if (id_persona <= 0 || ocupados == cap_sala) {RETURN(-1);}
    for (int i = 0; i < cap_sala; i++) {
        if (*(sala+i) == 0) {
            *(sala+i) = id_persona;
            ocupados++;
            RETURN(i + 1);
        }
    }
    RETURN(-1);
}


/**
 * Marca un asiento como libre.
 * @param id_asiento identificador del asiento a liberar
 * @return identificador de la persona que ocupaba el asiento, -1 si estaba libre o con cualquier otro error
 */
int libera_asiento(int id_asiento) {
    pthread_mutex_lock(&mutex);
    if (id_asiento > cap_sala || id_asiento <= 0 || *(sala + (id_asiento - 1) ) == 0) {RETURN(-1)};

    int result = *(sala + (id_asiento - 1));
    *(sala + (id_asiento - 1)) = 0;
    ocupados--;
    RETURN(result);
}


/**
 * Informa del estado del asiento pasado como argumento.
 * @param id_asiento identificador del asiento
 * @return identificador de quien ocupa el asiento, 0 si el asiento está libre, -1 en otro caso
 */
int estado_asiento(int id_asiento){
    pthread_mutex_lock(&mutex);
    if (id_asiento > cap_sala || id_asiento <= 0) {RETURN(-1);}
    RETURN(*(sala + id_asiento - 1));
}


/**
 * Devuelve el número de asientos libres.
 * @return número de asientos libres, -1 si hay un error
 */
int asientos_libres() {
    pthread_mutex_lock(&mutex);
    if (sala == NULL) {RETURN(-1)};
    RETURN(cap_sala - ocupados);
}


/**
 * Devuelve el número de asientos ocupados.
 * @return número de asientos ocupados, -1 si hay un error
 */
int asientos_ocupados() {
    pthread_mutex_lock(&mutex);
    if (sala == NULL) {RETURN(-1)};
    RETURN(ocupados);
}


/**
 * Devuelve la capacidad de la sala.
 * @return capacidad de la sala, -1 si hay un error
 */
int capacidad_sala() {
    pthread_mutex_lock(&mutex);
    if (sala == NULL) return(-1);
    RETURN(cap_sala);
}


/**
 * Crea una sala (puntero a int) con una capacidad dada.
 * @param capacidad capacidad que debe soportar la sala.
 * @return capacidad de sala si se ha podido crear, -1 en caso de error
 */
int crea_sala(int capacidad) {
    elimina_sala();
    pthread_mutex_lock(&mutex);
    if (capacidad <= 0) {RETURN(-1)};
    sala = calloc(capacidad, sizeof(int));
    if (sala == NULL) {RETURN(-1);}
    cap_sala = capacidad;
    ocupados = 0;
    RETURN(capacidad);
}


/**
 * Libera la memoria asociada a la sala.
 * @return 0 si se elimina correctamente, -1 en caso de error
 */
int elimina_sala() {
    pthread_mutex_lock(&mutex);
    if (sala == NULL) {RETURN(-1);}
    free(sala);
    sala = NULL;
    cap_sala = 0;
    ocupados = 0;
    RETURN(0);
}


/* Para asegurarnos de que la API sea thread-safe, incluimos aquí las funciones de aux_test_sala que podrían generar inconsistencias */

/**
 * Muestra el estado de todos y cada uno de los asientos
 * y una información resumen de la sala.
 */
int estado_sala(char* titulo) {
	pthread_mutex_lock(&mutex);

    if (sala == NULL) {
        puts("La sala no existe.\n-------------------\n");
        RETURN(-1);
    }

    printf("%s\n", titulo);

    printf("Estado de los asientos: {");

    for (int i = 0; i < cap_sala - 1; i++) printf("%d, ", *(sala + i));
    printf("%d} \n", *(sala + cap_sala - 1));

    printf("- Asientos libres = %d \n- Asientos ocupados = %d \n- Capacidad de la sala = %d \n-------------------\n",
           cap_sala - ocupados, ocupados, cap_sala);

    RETURN(0);
}