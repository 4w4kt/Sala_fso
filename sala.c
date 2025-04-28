//
// Created by aleja on 01/03/2025.
//

#include "sala.h"
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
    if (id_persona <= 0 || ocupados == cap_sala) return -1;
    for (int i = 0; i < cap_sala; i++) {
        if (*(sala+i) == 0) {
            *(sala+i) = id_persona;
            ocupados++;
            return i + 1;
        }
    }
    return -1;
}


/**
 * Marca un asiento como libre.
 * @param id_asiento identificador del asiento a liberar
 * @return identificador de la persona que ocupaba el asiento, -1 si estaba libre o con cualquier otro error
 */
int libera_asiento(int id_asiento) {
    if (id_asiento > cap_sala || id_asiento <= 0 || *(sala + (id_asiento - 1) ) == 0) return -1;

    int result = *(sala + (id_asiento - 1));
    *(sala + (id_asiento - 1)) = 0;
    ocupados--;
    return result;
}


/**
 * Informa del estado del asiento pasado como argumento.
 * @param id_asiento identificador del asiento
 * @return identificador de quien ocupa el asiento, 0 si el asiento está libre, -1 en otro caso
 */
int estado_asiento(int id_asiento){
    if (id_asiento > cap_sala || id_asiento <= 0) return -1;
    return *(sala + id_asiento - 1);
}

int set_asiento(int id_asiento, int id_persona){
    if (id_asiento > cap_sala || id_asiento <= 0) return -1;
    if(*(sala + id_asiento - 1) == 0 && id_persona > 0) {
        *(sala + id_asiento - 1) = id_persona;
        ocupados++;
        return 0;
    }
    if(*(sala + id_asiento - 1) != 0 && id_persona == 0){
        ocupados--;
        return 0;
    }
}

/**
 * Devuelve el número de asientos libres.
 * @return número de asientos libres, -1 si hay un error
 */
int asientos_libres() {
    if (sala == NULL) return -1;
    return cap_sala - ocupados;
}


/**
 * Devuelve el número de asientos ocupados.
 * @return número de asientos ocupados, -1 si hay un error
 */
int asientos_ocupados() {
    if (sala == NULL) return -1;
    return ocupados;
}


/**
 * Devuelve la capacidad de la sala.
 * @return capacidad de la sala, -1 si hay un error
 */
int capacidad_sala() {
    if (sala == NULL) return -1;
    return cap_sala;
}


/**
 * Crea una sala (puntero a int) con una capacidad dada.
 * @param capacidad capacidad que debe soportar la sala.
 * @return capacidad de sala si se ha podido crear, -1 en caso de error
 */
int crea_sala(int capacidad) {
    elimina_sala();
    if (capacidad <= 0)return -1;
    sala = calloc(capacidad, sizeof(int));
    if (sala == NULL) return -1;
    cap_sala = capacidad;
    ocupados = 0;
    return capacidad;
}

int reemplaaza_sala(int* asientos, int capacidad, int asientos_ocupados) {
    if (sala == NULL) return -1;
    free(sala);
    int* sala = asientos;
    if (sala == NULL) return -1;
    cap_sala = capacidad;
    ocupados = asientos_ocupados;
    return capacidad;
}

int* get_sala(){
    return sala;
}


/**
 * Libera la memoria asociada a la sala.
 * @return 0 si se elimina correctamente, -1 en caso de error
 */
int elimina_sala() {
    if (sala == NULL) return -1;
    free(sala);
    sala = NULL;
    cap_sala = 0;
    ocupados = 0;
    return 0;
}

