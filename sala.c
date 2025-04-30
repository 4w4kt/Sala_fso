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
    if (id_asiento > cap_sala || id_asiento <= 0 || id_persona < 0) return -1;
    
    if(*(sala + id_asiento - 1) == 0 && id_persona > 0) ocupados++;
    else if(*(sala + id_asiento - 1) != 0 && id_persona == 0) ocupados--;
        
    *(sala + id_asiento - 1) = id_persona;
    return 0;
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

int reemplaza_sala(int* asientos, int capacidad, int asientos_ocupados) {
    if (sala == NULL) return -1;
    free(sala);
    sala = asientos;
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






//adicion de la practica 3
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include "sala.h"


int guarda_estado_sala(char* ruta_fichero){
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
	int fd = open(ruta_fichero, O_RDONLY);
	CHECK_ERROR(fd);
	SELECT_DATOS_SALA(fd, 1);
	
	int* estado_asiento = calloc(datos_sala[0], sizeof(int));
	
	if(estado_asiento == NULL){
		close(fd);
		return -1;
	}
	
	bytes_leidos = read(fd, estado_asiento, sizeof(int)* datos_sala[0]);
	CHECK_LEIDO(bytes_leidos);
	close(fd);
	reemplaza_sala(estado_asiento, datos_sala[0], datos_sala[1]);
}


int guarda_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos){
	int fd = open(ruta_fichero, O_RDWR);
	CHECK_ERROR(fd);
	SELECT_DATOS_SALA(fd, 1);

	int estado_asiento_antiguo;
	int asientos_ocupados = datos_sala[1];

	for(int i = 0; i < num_asientos; i++){
		lseek(fd, sizeof(int)* (id_asientos[i] + 1), SEEK_SET);
		ssize_t bytes_leidos = read(fd, &estado_asiento_antiguo, sizeof(int));
		CHECK_LEIDO(bytes_leidos);

		lseek(fd, sizeof(int)* (id_asientos[i] + 1), SEEK_SET);
		int estado = estado_asiento(id_asientos[i]);
		int bytes_escritos = write(fd, &estado, sizeof(int));
		CHECK_ESCRITO(bytes_escritos);
		if(estado_asiento_antiguo == 0 && estado_asiento != 0){
			asientos_ocupados++;
			continue;
		}
		if(estado_asiento_antiguo != 0 && estado_asiento == 0){
			asientos_ocupados--;
		}
	}
	lseek(fd, sizeof(int)* 2, SEEK_SET); //cambiamos el valor de los asientos ocupados
	ssize_t bytes_escritos = write(fd, &asientos_ocupados, sizeof(int));
	CHECK_ESCRITO(bytes_escritos);
	close(fd);
	return 0;
}

int recupera_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos){
	int fd = open(ruta_fichero, O_RDWR);
	CHECK_ERROR(fd);
	SELECT_DATOS_SALA(fd, 1);
	
	int estado_asiento_antiguo;
	for(int i = 0; i < num_asientos; i++){
		lseek(fd, sizeof(int)* id_asientos[i], SEEK_SET);
		ssize_t bytes_leidos = read(fd, &estado_asiento_antiguo, sizeof(int));
		CHECK_LEIDO(bytes_leidos);
		set_asiento(id_asientos[i], estado_asiento_antiguo);
	}
	lseek(fd, sizeof(int)* 2, SEEK_SET); //cambiamos el valor de los asientos ocupados
	return 0;
}

/*
int main(int argc, char* argv[]){

	if(argc < 3){
	fprintf(stderr, "Error faltan argumentos, ejemplo de uso: %s archivo_1.txt  archivo_2.txt\n", argv[0]);
	return -1;
	}
	if(!strcmp(argv[3], "-f")){
		printf("hemos detectado la f");
		F_Status = 1;
	}
	copiar(argv[1], argv[2]);
	
	guarda_estado_sala(rutafichero);
}
*/



