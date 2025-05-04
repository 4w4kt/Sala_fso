#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "sala.h"
#include "macros.h"

#define MAX_BUFFER_SIZE 10000

#define RESERVAS_INVALIDAS\
	char* result = malloc(MAX_BUFFER_SIZE);\
	if (!result) {CIERRA_ANULA(1, "Error en la alocación de memoria");}\
	\
	char* ptr = result;\
	int entradilla;\
	\
	if (asientos_personas < 0) entradilla = sprintf(ptr, "Identificadores inválidos: ");\
	else entradilla = sprintf(ptr, "Asientos inválidos: ");\
	\
	ptr += entradilla;\
	*ptr++ = '{';\
	int numero;\
	\
	for (int i = 0; i < n_asientos_invalidos; i++) {\
		numero = sprintf(ptr, "%d, ", *(asientos_invalidos + i));\
		ptr += numero;\
	}\
	\
	*(ptr - 2) = '}';\
	*(ptr - 1) = '\n';\
	*ptr = '\0';


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
			fprintf(stderr, "Error: La ruta ya existe pero no se ha indicado la opción de sobreescritura\n");
			exit(1);
		}

		CREA_SALA(cap, 0);
		GUARDA(0);
		elimina_sala();
		exit(0);
	}
	
	if (!strcmp(option, "reserva")) {
	
		CHECK_NARGUMENTOS(5, 0);
		
		int opt = getopt(argc, argv, "f:");
		if (opt == -1) {
			fprintf(stderr, "Error: Lectura de ruta incorrecta.\n");
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
				ABORTAR(asientos, fd, "Error: Ha intentado reservar más asientos de los disponibles.\n")
			}
			*(asientos + n_asientos - 1) = atoi(argv[i]);
		}
		
		if (n_asientos == 0) {
			ABORTAR(asientos, fd, "Error: Reserva de los asientos fallida (consulte el orden de los argumentos)\n")
		}
		
		RECUPERA;
		
		if (reserva_multiple(n_asientos, asientos) == -1) {
			ABORTAR(asientos, fd, "Error: Reserva de los asientos fallida (consulte los IDs a reservar)\n")
		}
		
		GUARDA(1);
		
		free(asientos);
		asientos = NULL;
		elimina_sala();
		exit(0);
	}

	if (!strcmp(option, "anula")) {
	
		char** argv_copy = malloc(argc*sizeof(char*));
		for (int i = 0; i < argc; i++) {
			*(argv_copy + i) = argv[i];
		}
		
		int opt;
	    	int fd, capacidad, n_asientos, n_asientos_invalidos;
	    	int* asientos;
		int* asientos_invalidos;
		
		struct option longopts[] = {
			{"asientos", no_argument, NULL, 'a'},
			{"personas", no_argument, NULL, 'p'},
			{0, 0, 0, 0}
		};

		int asientos_personas = 0;
		int f = 0;
		int start = 0;
		opterr = 0;
		optind = 2;
		
		while ((opt = getopt_long_only(argc, argv, "f:", longopts, NULL)) != -1) {
			if (opt == 'f') {
				dir = optarg;
			    
			    	fd = open(dir, O_RDONLY);
				CHECK_ERROR(fd);
				
				SELECT_DATOS_SALA(fd, 0);
				capacidad = datos_sala[0];
				CREA_SALA(capacidad, 1);
				
				asientos = malloc(capacidad * sizeof(int));
				n_asientos = 0;

				asientos_invalidos = malloc(capacidad * sizeof(int));
				n_asientos_invalidos = 0;

				if (asientos == NULL || asientos_invalidos == NULL) {CIERRA_ANULA(1, "Error en la alocación de memoria");}
				f = optind - 2;
				continue;
			
			}
			if (opt == 'a') {
				if (asientos_personas == -1) {CIERRA_ANULA(0, "Ha intentado anular reservas de asientos y personas a la vez\n");}
			        asientos_personas = 1;
			        start = optind;
			        continue;
			}
			if (opt == 'p') {
				if (asientos_personas == 1) {CIERRA_ANULA(0, "Ha intentado anular reservas de asientos y personas a la vez\n");}
				asientos_personas = -1;
				start = optind;
				continue;
			}
		}
		
		if (!f) {
			fprintf(stderr, "Error: Lectura de ruta incorrecta.\n");
			exit(1);
		}
		
		if (!asientos_personas) {CIERRA_ANULA(0, "No se han indicado correctamente las reservas a anular (asientos/personas)\n");}
		
		if (asientos_personas > 0) {
			if (start < f) {
				for (int i = start; i < f; i++) {ASIENTO_CORRECTO(atoi(argv_copy[i]));}
			} else {
				for (int i = start; i < argc; i++) {ASIENTO_CORRECTO(atoi(argv_copy[i]));}
			}
			if (n_asientos > 0) {
				if (recupera_estado_parcial_sala(dir, n_asientos, asientos) == -1) {CIERRA_ANULA(0, "Error al recuperar el estado de la sala\n");}
				for (int i = 0; i < n_asientos; i++) libera_asiento(*(asientos + i));
			}
		} else {
			if (recupera_estado_sala(dir) == -1) {CIERRA_ANULA(0, "Error al recuperar el estado de la sala\n");}
			if (start < f) {
				for (int i = start; i < f; i++) {PERSONA_CORRECTA(atoi(argv_copy[i]));}
			} else {
				for (int i = start; i < argc; i++) {PERSONA_CORRECTA(atoi(argv_copy[i]));}
			}
		}
		
		if (n_asientos > 0 && guarda_estado_parcial_sala(dir, n_asientos, asientos) == -1) {CIERRA_ANULA(0, "Se produjo un error al guardar el estado de la sala\n");}
		
		if (n_asientos_invalidos > 0) {
		RESERVAS_INVALIDAS;
		CIERRA_ANULA(0, result);
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
		
		
		RECUPERA;
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
			free(sala_1); free(sala_2);
			close(fd1); close(fd2);
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
