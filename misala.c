#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "sala.h"

#define MAX_BUFFER_SIZE 10000

#define ASIENTO_CORRECTO(x)\
	if (x >= capacidad || x <= 0) {\
		*(asientos_invalidos + n_asientos_invalidos) = x;\
		 n_asientos_invalidos++;\
	} else {\
		*(asientos + n_asientos) = x;\
		n_asientos++;\
	}
			
#define PERSONA_CORRECTA(x)\
 	int asiento_ocupado = levantarse(x);\
 	if (asiento_ocupado == -1) {\
 		*(asientos_invalidos + n_asientos_invalidos) = x;\
 		n_asientos_invalidos++;\
 	} else {\
 		*(asientos + n_asientos) = asiento_ocupado;\
 		n_asientos++;\
 	}
 	
char* reservas_invalidas(int* reservas, int n_reservas, int personas) {
	char* result = malloc(MAX_BUFFER_SIZE);
	if (!result) return NULL;
	
	char* ptr = result;
	int entradilla;
	
	if (personas < 0) entradilla = sprintf(ptr, "Identificadores inválidos: ");
	else entradilla = sprintf(ptr, "Asientos inválidos: ");
	
	ptr += entradilla;
	*ptr++ = '{';
	
	for (int i = 0; i < n_reservas; i++) {
		int numero = sprintf(ptr, "%d, ", reservas[i]);
		ptr += numero;
	}
	
	*(ptr - 2) = '}';
	*(ptr - 1) = '\0';
	return result;
}

int main(int argc, char* argv[]) {

	if (argc < 4) {
		perror("Número de argumentos incorrecto");
		exit(1);
	}

	char* option = argv[1];
	char* dir;
	
	if (!strcmp(option, "crea")) {
		int opt;
		int override = 0;
		int cap;
		
		while ((opt = getopt(argc, argv, "f:c:")) != -1) {
			
			if (opt == 'f') {
				
				if (!strcmp(optarg, "o")) {
					override = 1;
					dir = argv[optind];
					continue;
				}
				dir = optarg;
				continue;
			}
			
			if (opt == 'c') {
				cap = atoi(optarg);
				continue;
			}
			
		}

		
		if (override) {
			if (access(dir, W_OK) != 0 && errno == EACCES) {
				perror("No tiene permisos de escritura para la ruta indicada");
				exit(1);
			}
		} else if (!access(dir, F_OK)) {
			perror("La ruta ya existe pero no se ha indicado la opción de sobreescritura");
			exit(1);
		}

		if (crea_sala(cap) == -1) {
			perror("No se pudo crear la sala");
			exit(1);
		}
		if (guarda_estado_sala(dir) == -1) {
			elimina_sala();
			perror("No se pudo guardar el estado de la sala en la ruta indicada");
			exit(1);
		}
		elimina_sala();
		exit(0);
	}
	
	if (!strcmp(option, "reserva")) {
		
		int opt = getopt(argc, argv, "f:");
		if (opt == -1) {
			perror("Lectura de ruta incorrecta");
			exit(1);
		}
		
		dir = optarg;
		
		int fd = open(dir, O_RDONLY);
		CHECK_ERROR(fd);
		SELECT_DATOS_SALA(fd, 0);
		
		if (crea_sala(datos_sala[0]) == -1) {
			perror("No se pudo crear la sala");
			exit(1);
		}
	
		int max_asientos = datos_sala[0] - datos_sala[1];
		int* asientos = malloc(max_asientos * sizeof(int));
		int n_asientos = 0;
		
		if (asientos == NULL) {
			perror("Error en la alocación de memoria");
			elimina_sala();
			close(fd);
			exit(-1);
		}

		for (int i = optind; i < argc; i++) {
			n_asientos++;
			if (n_asientos > max_asientos) {
				free(asientos);
				asientos = NULL;
				close(fd);
				
				perror("Ha intentado reservar más asientos de los disponibles");
				exit(1);
			}
			*(asientos + n_asientos - 1) = atoi(argv[i]);
		}
		
		if (recupera_estado_sala(dir) == -1) {
			elimina_sala();
			perror("No se pudo recuperar el estado de la sala");
			close(fd);
			exit(1);
		}
		
		if (reserva_multiple(n_asientos, asientos) == -1) {
			elimina_sala();
			perror("Reserva de los asientos fallida");
			exit(1);
		}
		
		if (guarda_estado_sala(dir) == -1) {
			elimina_sala();
			perror("No se pudo guardar la sala actualizada");
			close(fd);
			exit(1);
		}
		
		elimina_sala();
		exit(0);
	}

	if (!strcmp(option, "anula")) {
	
	    int opt, fd, capacidad, n_asientos, n_asientos_invalidos;
	    int* asientos;
	    int* asientos_invalidos;
	    
		struct option longopts[] = {
			{"asientos", required_argument, NULL, 'a'},
			{"personas", required_argument, NULL, 'p'},
			{0, 0, 0, 0}
		};

		int asientos_personas = 0;
		optind = 2;
		
		while ((opt = getopt_long_only(argc, argv, "f:", longopts, NULL)) != -1) {
	        if (opt == 'f') {
	                dir = optarg;
	                		
					fd = open(dir, O_RDONLY);
					CHECK_ERROR(fd);
					SELECT_DATOS_SALA(fd, 0);
					capacidad = datos_sala[0];
					
					if (crea_sala(capacidad) == -1) {
						perror("No se pudo crear la sala");
						close(fd);
						exit(1);
					}
					
					asientos = malloc(capacidad * sizeof(int));
					n_asientos = 0;
		
					asientos_invalidos = malloc(capacidad * sizeof(int));
					n_asientos_invalidos = 0;
		
					if (asientos == NULL || asientos_invalidos == NULL) {
						elimina_sala();
						close(fd);
						perror("Error en la alocación de memoria");
						exit(1);
					}
					
	                continue;
	        }
	        if (opt == 'a') {
	        		if (asientos_personas == -1) {
	        			perror("Ha intentado anular reservas de asientos y personas a la vez");
	        			exit(1);
	        		}
	                asientos_personas = 1;
	                ASIENTO_CORRECTO(atoi(optarg));
	                continue;
	        }
	        if (opt == 'p') {
	        		if (asientos_personas == 1) {
	        			perror("Ha intentado anular reservas de asientos y personas a la vez");
	        			exit(1);
	        		}
	        		asientos_personas = -1;
	        		if (recupera_estado_sala(dir) == -1) {
	        			elimina_sala();
	        			perror("Error al recuperar el estado de la sala");
	        			exit(1);
	        		}
	        		PERSONA_CORRECTA(atoi(optarg));
	        		continue;
	        }
		}
		
		if (asientos_personas == 0) {
			perror("No se han indicado correctamente las reservas a anular (asientos/personas)");
			exit(1);
		}
		
		if (asientos_personas > 0) {
			for (int i = optind; i < argc; i++) {
					ASIENTO_CORRECTO(atoi(argv[optind]));
			}
			if (recupera_estado_parcial_sala(dir, n_asientos, asientos) == -1) {
				elimina_sala();
				perror("Error al recuperar el estado de la sala");
				exit(1);
			}
		} else {
			for (int i = optind; i < argc; i++) {
					PERSONA_CORRECTA(atoi(argv[i]));
			}
		}
		
		if (guarda_estado_parcial_sala(dir, n_asientos, asientos) == -1) {
			elimina_sala();
			perror("Se produjo un error al guardar el estado de la sala");
			exit(1);
		}
		elimina_sala();
		if (n_asientos_invalidos > 0) {
			perror(reservas_invalidas(asientos_invalidos, n_asientos_invalidos, asientos_personas));
			exit(1);
		}
		exit(0);
	}
	
	if (!strcmp(option, "estado")) {
		int n_asientos;
		
		int opt = getopt(argc, argv, "f:");
		if (opt == -1) {
			perror("Lectura de ruta incorrecta");
			exit(1);
		}
		
		dir = optarg;
		int fd = open(dir, O_RDONLY);
		CHECK_ERROR(fd);
		SELECT_DATOS_SALA(fd, 0);
		int accesos = 1;
		while (crea_sala(datos_sala[0] / accesos) == -1) accesos *= 2;
		
		if (accesos == 1) {
			recupera_estado_sala(dir);
			estado_sala("========= Estado de la sala =========");
			elimina_sala();
			close(fd);
			exit(0);
		}
			
		// falta comprobar el caso de que no se pueda cargar la sala
		/*
		accesos /= 2;
		printf("la capacidad de la sala es: %d y tiene %d asientos ocupados\n{ ", datos_sala[0], datos_sala[1]);
		int bytes_leidos = 1;
		while (bytes_leidos > 0){
			ssize_t bytes_leidos = read(fd, sala, sizeof(int) * (datos_sala_1[0] / accesos));
			CHECK_LEIDO(bytes_leidos);
			for (int i = 0; i < bytes_leidos/sizeof(int); i++) {
					printf("%d, ", *(sala + i));
				}
			}
			printf("}\n");
		close(fd);
		*/
	}


	if(!strcmp(option, "compara")) {
		if(!strcmp(argv[2], argv[3])){
			puts("son iguales");
			return 0;
		}
		
		int fd1 = open(argv[2], O_RDONLY);
		CHECK_ERROR(fd1);
		int fd2 = open(argv[3], O_RDONLY);
		CHECK_ERROR(fd2);
		int datos_sala_1[2];
		int datos_sala_2[2];
		ssize_t bytes_leidos = read(fd1, &datos_sala_1, sizeof(int)*2);
		CHECK_LEIDO(bytes_leidos); 
		ssize_t bytes_leidos2 = read(fd2, &datos_sala_2, sizeof(int)*2);
		CHECK_LEIDO(bytes_leidos2);
		if(datos_sala_1[0] != datos_sala_2[0] || datos_sala_1[1] != datos_sala_2[1]){
			printf("Las salas no son iguales\n");
			close(fd1);
			close(fd2);
			return 1;
			exit(1);
		}
		int accesos = 1;
		int* sala_1 = NULL;
		int* sala_2 = NULL;
		while (sala_1 == NULL || sala_2 == NULL) {
			sala_1 = malloc(sizeof(int) * (datos_sala_1[0] / accesos));
			sala_2 = malloc(sizeof(int) * (datos_sala_2[0] / accesos));
			accesos *= 2;
		}
		accesos /= 2;
		if((sala_1 == NULL  || sala_2 == NULL) && errno == ENOMEM){
			perror("Error al asignar memoria para las dos salas");
			exit(1);
		}
		bytes_leidos = 1;
		while (1){
			ssize_t bytes_leidos = read(fd1, sala_1, sizeof(int) * (datos_sala_1[0] / accesos));
			CHECK_LEIDO(bytes_leidos);
			ssize_t bytes_leidos2 = read(fd2, sala_2, sizeof(int) * (datos_sala_2[0] / accesos));
			CHECK_LEIDO(bytes_leidos2);
			if(bytes_leidos != bytes_leidos2){
					printf("Las salas no son iguales\n");
					close(fd1);
					close(fd2);
					free(sala_1);
					free(sala_2);
					return 1;
					exit(1);
			}
			if(bytes_leidos == 0) break;
			
			for (int i = 0; i < bytes_leidos2*sizeof(int) && i < datos_sala_2[0] + 2; i++) {
				if (*(sala_1 + i) != *(sala_2 + i)) {
					printf("Las salas no son iguales %d %d != %d\n", i, *(sala_1 + i) ,*(sala_2 + i) );
					close(fd1);
					close(fd2);
					free(sala_1);
					free(sala_2);
					return 1;
					exit(1);
				}
			}
		}
		puts("son iguales");
		return 0;
	}

}
