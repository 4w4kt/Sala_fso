//
// Created by aleja on 01/03/2025.
//

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>



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




/* Adición de la práctica 3 */


/**
 * Libera la memoria asociada a la sala.
 * @return 0 si se elimina correctamente, -1 en caso de error
 */
int set_asiento(int id_asiento, int id_persona){
    if (id_asiento > cap_sala || id_asiento <= 0 || id_persona < 0) return -1;
    
    if(*(sala + id_asiento - 1) == 0 && id_persona > 0) ocupados++;
    else if(*(sala + id_asiento - 1) != 0 && id_persona == 0) ocupados--;
        
    *(sala + id_asiento - 1) = id_persona;
    return 0;
}


int guarda_estado_sala(char* ruta_fichero){
	if (sala == NULL) return -1;
	int mode = O_CREAT | O_WRONLY | O_TRUNC;
	int fd = open(ruta_fichero, mode);
	CHECK_ERROR(fd);
	
	ssize_t bytes_escritos = write(fd, &cap_sala, sizeof(int));
	CHECK_ESCRITO(bytes_escritos);
	bytes_escritos = write(fd, &ocupados, sizeof(int));
	CHECK_ESCRITO(bytes_escritos); 
	bytes_escritos = write(fd, sala, sizeof(int) * cap_sala);
	CHECK_ESCRITO(bytes_escritos);
	close(fd);
	return 0;
}


int recupera_estado_sala(char* ruta_fichero){
	if (sala == NULL) return -1;
	int fd = open(ruta_fichero, O_RDONLY);
	CHECK_ERROR(fd);
	SELECT_DATOS_SALA(fd, 1);
	ocupados = datos_sala[1];
	bytes_leidos = read(fd, sala, sizeof(int)* cap_sala);
	CHECK_LEIDO(bytes_leidos);
	close(fd);
}


int guarda_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos){
	if (sala == NULL) return -1;
	
	int fd = open(ruta_fichero, O_RDWR);
	CHECK_ERROR(fd);
	SELECT_DATOS_SALA(fd, 1);

	int estado_asiento_antiguo, estado;
	int asientos_ocupados = datos_sala[1];
	ssize_t bytes_escritos;

	for(int i = 0; i < num_asientos; i++){
		lseek(fd, sizeof(int)* (id_asientos[i] + 1), SEEK_SET);
		bytes_leidos = read(fd, &estado_asiento_antiguo, sizeof(int));
		CHECK_LEIDO(bytes_leidos);

		lseek(fd, sizeof(int)* (id_asientos[i] + 1), SEEK_SET);
		estado = *(sala + id_asientos[i] - 1);
		bytes_escritos = write(fd, &estado, sizeof(int));
		CHECK_ESCRITO(bytes_escritos);
		if(estado_asiento_antiguo == 0 && estado != 0){
			asientos_ocupados++;
			continue;
		}
		if(estado_asiento_antiguo != 0 && estado == 0){
			asientos_ocupados--;
		}
	}
	
	lseek(fd, sizeof(int), SEEK_SET);
	bytes_escritos = write(fd, &asientos_ocupados, sizeof(int));
	CHECK_ESCRITO(bytes_escritos);
	close(fd);
	return 0;
}

int recupera_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos){
	int fd = open(ruta_fichero, O_RDONLY);
	CHECK_ERROR(fd);
	SELECT_DATOS_SALA(fd, 1);
	
	int estado_asiento_antiguo;
	for(int i = 0; i < num_asientos; i++){
		lseek(fd, sizeof(int)* (id_asientos[i] + 1), SEEK_SET);
		ssize_t bytes_leidos = read(fd, &estado_asiento_antiguo, sizeof(int));
		CHECK_LEIDO(bytes_leidos);
		if(bytes_leidos == 0) continue;
		set_asiento(id_asientos[i], estado_asiento_antiguo);
	}
	lseek(fd, sizeof(int), SEEK_SET);
	return 0;
}

