#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <stdlib.h>
#include "sala.h"

    

int guardar_estado_sala(char* ruta_fichero){
	int mode = O_CREAT | O_WRONLY | O_TRUNC;
	int fd = open(ruta_fichero, mode);
	CHECK_ERROR(fd);
	
	int capacidad_sala = capacidad_sala();
	ssize_t bytes_escritos = write(fd, &capacidad_sala, sizeof(int));
	CHECK_ESCRITO(bytes_escritos);
	ssize_t bytes_escritos = write(fd, &asientos_ocupados(), sizeof(int));
	CHECK_ESCRITO(bytes_escritos);
	ssize_t bytes_escritos = write(fd, &get_sala(), sizeof(int)*capacidad_sala);
	CHECK_ESCRITO(bytes_escritos);
	close(fd)
	return;
}


int recupera_estado_sala(char* ruta_fichero){	
	int fd = open(ruta_fichero, O_RDONLY);
	CHECK_ERROR(fd);
	
	SELECT_DATOS_SALA(fd);
	
	int* estado_asiento = malloc(sizeof(int) * capacidad_sala);
	ssize_t bytes_leidos = read(fd, &estado_asiento, sizeof(int));
	CHECK_LEIDO(bytes_leidos);
	close(fd);
	reemplaaza_sala(estado_asiento, datos_sala[0], datos_sala[1]);
}


int guarda_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos){
	int fd = open(ruta_fichero, O_RDWR);
	CHECK_ERROR(fd);
	SELECT_DATOS_SALA(fd);

	int estado_asiento_antiguo;
	int asientos_ocupados = datos_sala[1];

	for(int i = 0; i < num_asientos; i++){
		lseek(fd, sizeof(int)* id_asientos[i], SEEK_SET);
		ssize_t bytes_leidos = read(fd, &estado_asiento_antiguo, sizeof(int));
		CHECK_LEIDO(bytes_leidos);

		lseek(fd, sizeof(int)* id_asientos[i], SEEK_SET);
		estado_asiento = estado_asiento(id_asiento[i]);
		bytes_escritos = write(fd, &estado_asiento, sizeof(int));
		CHECK_ESCRITO(bytes_escritos);
		if(estado_asiento_antiguo == 0 && estado_asiento != 0){
			asientos_ocupados++;
			continue;
		}
		if(estado_asiento_antiguo != 0 && estado_asiento == 0){
			asientos_ocupados--;
		}
	}
	lseek(fd, sizeof(int)* 2, SEEK_SET) //cambiamos el valor de los asientos ocupados
	bytes_escritos = write(fd, &asientos_ocupados, sizeof(int));
	close(fd);
	return 0;
}

int recupera_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos){
	int fd = open(ruta_fichero, O_RDWR);
	CHECK_ERROR(fd);
	SELECT_DATOS_SALA(fd);
	
	int estado_asiento_antiguo;
	for(int i = 0; i < num_asientos; i++){
		lseek(fd, sizeof(int)* id_asientos[i], SEEK_SET);
		ssize_t bytes_leidos = read(fd, &estado_asiento_antiguo, sizeof(int));
		CHECK_LEIDO(bytes_leidos);
		set_asiento(id_asientos[i], estado_asiento_antiguo);
	}
	lseek(fd, sizeof(int)* 2, SEEK_SET) //cambiamos el valor de los asientos ocupados
	return 0;
}


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
	
	guardar_estado_sala(rutafichero);
}