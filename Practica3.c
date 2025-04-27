#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <stdlib.h>
#include "sala.h"

#define CHECK_LEIDO(x)         \
    if ((x) == -1) {            \
        perror("Error en la lectura"); \
        return -1;              \
    }
    
   
#define BUFFER_SIZE 512

int f_Status =0;

int cerrar(int fd){
	if(close(fd) == -1){
	perror("Error");
	return -1;
	}
	puts("Se cerró el fichero correctamente");
	return 0;
}



int copiar(char* emisor, char* receptor){
	int fd_reader = open(emisor, O_RDONLY);
	if (fd_reader == -1){
		perror("Error");
		return -1;
	}
	int mode = O_WRONLY | O_TRUNC;
	if (!F_Status){
		if (access(receptor, F_OK) == -1){
			perror("Error");
			return -1;
		}
		puts("¿Desea sobreescribir el fichero?\n");
		char* respuesta;
		do{
		puts("responda: \"Si\" | \"No\"");
		scanf("%s", &repuesta);
		} while(!strcmp(respuesta, "Si") ||while(!strcmp(respuesta, "No")))
		if(!strcmp(respuesta, "No")){
			mode = O_WRONLY;
		}
	}
	
	int fd_writer = open(receptor, mode);
	if (fd_writer == -1){
		perror("Error");
		return -1;
	}
	
	char* buffer = malloc(BUFFER_SIZE); 
	ssize_t bytes_leidos = read(fd_reader, buffer, BUFFER_SIZE);
	int counter;
	while (bytes_leidos > 0){
		while(counter < bytes_leidos){
			ssize_t written = write(fd_writer, buffer, bytes_leidos);
			if (written == -1){
				perror("Error en la lectura");
				return -1;
			}
			counter += written;
		}
	}
	if(bytes_leidos == -1){
		perror("Error en la lectura");
		return -1;
	}
	printf("se ha copiado %s en %s", emisor, receptor);
	
	cerrar(fd_writer);
	cerrar(fd_reader);
	
}

guardar_estado_sala(char* direccion){
	int mode = O_CREAT | O_WRONLY | O_TRUNC;
	int fd_writer = open(direccion, mode);
	if (fd_writer == -1){
		perror("Error");
		return -1;
	}
	
	int capacidad_sala = capacidad_sala();
	int asientos_ocupados = asientos_ocupados();
	ssize_t bytes_escritos = write(fd, &capacidad_sala, sizeof(numero));
	if(bytes_escritos == -1){
		perror("error escribiendo en el archivo");
		close(fd);
		return -1;
	}
	
	ssize_t bytes_escritos = write(fd, &asientos_ocupados, sizeof(numero));
	if(bytes_escritos == -1){
		perror("error escribiendo en el archivo");
		close(fd);
		return -1;
	}
	int counter = 0;
	//completar con matrices mudas
	if (asientos_ocupados * 2 +1< capacidad_sala){
		
		for(int i = 0; counter < asientos_ocupados; i++){
			estado = estado_asiento(i)
			if(estado > 0) {
				ssize_t bytes_escritos = write(fd, &estado, sizeof(numero));
				if(bytes_escritos == -1){
					perror("error escribiendo en el archivo");
					close(fd);
					return -1;
				}
				counter++;
			}
		}
	}
	//si está ocupado la mayoría:
	for(int i = 0; counter < asientos_ocupados; i++){
		estado = estado_asiento(i)
		ssize_t bytes_escritos = write(fd, &estado, sizeof(numero));
		if(bytes_escritos == -1){
			perror("error escribiendo en el archivo");
			close(fd);
			return -1;
		}
	}
	close(fd)

}
int recupera_estado_sala (char* ruta_fichero){
	if(capacidad_sala() < 1){
		perror("La sala no existe");
		return -1;
	}
	
	int fd = open(ruta_fichero, O_RDONLY);
	if (fd_reader == -1){
		perror("Error");
		return -1;
	}
	
	int datos_sala [2];
	for(int i = 0; i<2; i++){
		ssize_t bytes_leidos = read(fd, &datos_sala, sizeof(int));
		
	} 
	ssize_t bytes_leidos = read(fd_reader, buffer, BUFFER_SIZE);
	
	if(bytes_leidos == -1){
		perror("Error en la lectura");
		return -1;
	}
	int counter = 0;
	while (bytes_leidos > 0){
		
		ssize_t bytes_leidos = read(fd_reader, buffer, BUFFER_SIZE);
		
	}

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