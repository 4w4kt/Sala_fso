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

extern pthread_mutex_t mutex;

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

/* Para asegurarnos de que la API sea thread-safe, incluimos aquí las funciones de aux_test_sala que podrían generar inconsistencias */

/**
 * Muestra el estado de todos y cada uno de los asientos
 * y una información resumen de la sala.
 */
int estado_sala(char* titulo) {
	pthread_mutex_lock(&mutex);

    if (sala == NULL) {
        pthread_mutex_unlock(&mutex);
        puts("La sala no existe.\n-------------------\n");
        return(-1);
    }

    printf("%s\n", titulo);

    printf("Estado de los asientos: {");

    for (int i = 0; i < cap_sala - 1; i++) printf("%d, ", *(sala + i));
    printf("%d} \n", *(sala + cap_sala - 1));

    printf("- Asientos libres = %d \n- Asientos ocupados = %d \n- Capacidad de la sala = %d \n-------------------\n",
           cap_sala - ocupados, ocupados, cap_sala);

    RETURN(0);
}


/**
 * Libera un asiento ocupado por la persona pasado por parametro
 * @param id_persona identificador de la persona que va a liberar el asiento
 * @return número del asiento liberado, -1 en caso contrario
 */
int levantarse(int id_persona) {
    pthread_mutex_lock(&mutex);
    if (id_persona <= 0 || ocupados == 0) {
        pthread_mutex_unlock(&mutex);
        if (DETALLES) printf("Ha ocurrido un error.\n");
        return(-1);
    }

    for (int i = 0; i < cap_sala; i++) {
        if (*(sala + i) == id_persona) {
            *(sala + i) = 0;
            ocupados--;
            pthread_cond_signal(&mutex);
            if (DETALLES) printf("%d se ha levantado del asiento %d\n", id_persona, i);
            return(i);
        }
    }
    pthread_mutex_unlock(&mutex);
    if (DETALLES) printf("No se ha encontrado %d en esta sala\n", id_persona);
    return(-1);
}


/**
 * Encuentra un asiento para la persona pasada por parámetro
 * @param id_persona identificador de la persona que va a ocupar el asiento
 * @return el número de asiento donde se puede sentar, -1 en si no es posible asignarle un asiento
 */
int sentarse(int id_persona) {
    int result = reserva_asiento(id_persona);
    if (DETALLES) {
        if (result != -1) {

            printf("Puede sentarse en el asiento %d, %d\n", result, id_persona);
            return result;
        }
        printf("No se ha encontrado un asiento para usted, %d\n", id_persona);
        return result;
    }
    return result;

}

/**
 * Reserva un asiento a cada persona pasada
 * @param n_personas cantidad de personas en la lista
 * @param lista_id lista de personas que desean un asiento
 * @return el número de personas si se a encontrado asiento para todas, -1 en caso contrario
 */

int reserva_multiple(int n_personas, int* lista_id) {

    for (int i = 0; i < n_personas; i++) {
        if (*(lista_id + i) <= 0) {
            if (DETALLES) puts("No se pudo realizar la reserva."); 
            return(-1);
        }
    }

    pthread_mutex_lock(&mutex);

    if (cap_sala < n_personas + ocupados || n_personas <= 0) {
        pthread_mutex_unlock(&mutex);
        if (DETALLES) puts("No se pudo realizar la reserva.");
        return(-1);
    }

    int asientos[n_personas];
    int j = 0;

    for (int i = 0; i < n_personas; i++) {
        if (*(sala + i) == 0){
            asientos[j] = i + 1;
            *(sala + i) = *(lista_id + j);
            ocupados++;
            if (++j == n_personas) break;
        }
    }

    pthread_mutex_unlock(&mutex);
    
    if (DETALLES){
        printf("Los asientos reservados fueron {");
        for (int i = 0; i<n_personas -1; i++){
            printf("%d, ", asientos[i]);
        }
        printf("%d} \n", asientos[n_personas-1]);
    }

    return(n_personas);
}
