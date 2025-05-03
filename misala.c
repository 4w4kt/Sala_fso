#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "sala.h"
#include "macros.h"

#define MAX_BUFFER_SIZE 10000

 	
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

	char* option = argv[1];
	char* dir;
	
	if (!strcmp(option, "crea")) {
		int opt;
		int override = 0;
		int cap = 0;
		
		CHECK_NARGUMENTOS(6, 1);
		
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

		CREA_SALA(cap, 0);
		GUARDA(1, 0);
		elimina_sala();
		exit(0);
	}
	
	if (!strcmp(option, "reserva")) {
	
		CHECK_NARGUMENTOS(6, 0);
		
		int opt = getopt(argc, argv, "f:");
		if (opt == -1) {
			perror("Lectura de ruta incorrecta");
			exit(1);
		}
		
		dir = optarg;
		
		int fd = open(dir, O_RDONLY);
		CHECK_ERROR(fd);
		SELECT_DATOS_SALA(fd, 0);
		 
		CREA_SALA(datos_sala[0], 1);
	
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
		
		RECUPERA(1, 1);
		
		if (reserva_multiple(n_asientos, asientos) == -1) {
			elimina_sala();
			perror("Reserva de los asientos fallida");
			exit(1);
		}
		
		GUARDA(1, 1);
		
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
					CREA_SALA(datos_sala[0], 1);
					
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
	        if (opt == '?') {
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
			for (int i = 0; i < n_asientos; i++) {
				libera_asiento(*(asientos + i));
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
	
		int opt = getopt(argc, argv, "f:");
		if (opt == -1) {
			perror("Lectura de ruta incorrecta");
			exit(1);
		}
		
		dir = optarg;
		int fd = open(dir, O_RDONLY);
		CHECK_ERROR(fd);
		
		SELECT_DATOS_SALA(fd, 0);
		CREA_SALA(datos_sala[0], 1);
		
		
		RECUPERA(1, 1);
		estado_sala("========= Estado de la sala =========");
		elimina_sala();
		close(fd);
		exit(0);
	}


	if(!strcmp(option, "compara")) {
	
		if(!strcmp(argv[2], argv[3])){
			puts("Las salas son iguales.");
			exit(0);
		}
		
		int fd1 = open(argv[2], O_RDONLY);
		CHECK_ERROR(fd1);
		
		int fd2 = open(argv[3], O_RDONLY);
		CHECK_ERROR(fd2);
		
		int datos_sala_1[2];
		ssize_t bytes_leidos = read(fd1, &datos_sala_1, sizeof(int)*2);
		CHECK_LEIDO(bytes_leidos); 
		
		int datos_sala_2[2];
		ssize_t bytes_leidos2 = read(fd2, &datos_sala_2, sizeof(int)*2);
		CHECK_LEIDO(bytes_leidos2);
		
		if(datos_sala_1[0] != datos_sala_2[0] || datos_sala_1[1] != datos_sala_2[1]){
			puts("Las salas no son iguales");
			close(fd1); close(fd2);
			exit(1);
		}
		
		int* sala_1 = malloc(sizeof(int) * datos_sala_1[0]);
		int* sala_2 = malloc(sizeof(int) * datos_sala_2[0]);

		if((sala_1 == NULL  || sala_2 == NULL) && errno == ENOMEM){
			perror("Error en la alocación de memoria");
			exit(1);
		}
		
		bytes_leidos = 1;
		bytes_leidos2 = 1;
		
		while (bytes_leidos && bytes_leidos2){
		
			bytes_leidos = read(fd1, sala_1, sizeof(int) * datos_sala_1[0]);
			CHECK_LEIDO(bytes_leidos);
			
			bytes_leidos2 = read(fd2, sala_2, sizeof(int) * datos_sala_2[0]);
			CHECK_LEIDO(bytes_leidos2);
			
			DISTINTAS(bytes_leidos != bytes_leidos2);
			
			for (int i = 0; i < bytes_leidos2 / sizeof(int); i++) {
				DISTINTAS(*(sala_1 + i) != *(sala_2 + i));
			}
		}
		
		DISTINTAS(bytes_leidos != bytes_leidos2);
		puts("Las salas son iguales.");
		exit(0);
	}
}
