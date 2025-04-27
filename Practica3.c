#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <stdlib.h>
#include "sala.h"

#define CHECK_LEIDO(x)         \
    if ((x) == -1) {            \
        perror("Error en la lectura"); \
		close(x)
        return -1;              \
    }

#define CHECK_ERROR(x)         \
	   if ((x) == -1) {            \
	   perror("Error en la lectura"); \
	   return -1;              \
	}

#define CHECK_ESCRITO(x)         \
	   if ((x) == -1) {            \
	   perror("Error en escribiendo en el archivo");		\
		close(x);
		return -1;
	}
    

void guardar_estado_sala(char* ruta_fichero){
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


void recupera_estado_sala(char* ruta_fichero){	
	int fd = open(ruta_fichero, O_RDONLY);
	CHECK_ERROR(fd);
	
	int datos_sala [2];
	for(int i = 0; i<2; i++){
		ssize_t bytes_leidos = read(fd, &datos_sala, sizeof(int));
		CKECK_LEIDO(bytes_leidos);
		if(capacidad_sala() != datos_sala[0] ){
			perror("La sala creada tiene una capacidad distinta a la que quiere restaurar");
			return -1;
		}
	}
	int* estado_asiento = malloc(sizeof(int) * capacidad_sala);
	ssize_t bytes_leidos = read(fd, &estado_asiento, sizeof(int));
	CHECK_LEIDO(bytes_leidos);
	close(fd);
	reemplaaza_sala(estado_asiento, datos_sala[0], datos_sala[1]);
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