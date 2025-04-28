#include <unistd.h>
#include <getopt.h>

int main(int argc, int argv) {

	if (argc < 4) {
		perror("Número de argumentos incorrecto");
		exit(1);
	}

	char* option = argv[1];
	
	if (!strcmp(option, "crea")) {
		int opt;
		char* dir;
		int override = 0;
		int cap;
		
		while (opt = getopt(argc, argv, "f:c:") != -1) {
		
			if (opt == 'f') {
				if (optarg == "o") {
					override = 1;
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
		
			while (opt = getopt(argc, argv, "fo:c:") != -1) {
			
				if (opt == 'o') {
					dir = optarg;
					break;
				}	
			}
		}

		// Gestionar archivo
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
		
		char* dir = optarg;
		
		int fd = open(dir, O_RDONLY);
		CHECK_ERROR(fd);
		SELECT_DATOS_SALA(fd, 0);
	
		int max_asientos = datos_sala[0] - datos_sala[1];
		int* asientos = malloc(max_asientos * sizeof(int));
		int n_asientos = 0;

		for (int i = optind; i < argc; i++) {
			n_asientos++;
			if (n_asientos > max_asientos) {
				free(asientos)
				asientos = NULL;
				close(fd);
				
				perror("Ha intentado reservar más asientos de los disponibles");
				exit(1);
			}
			*(asientos + n_asientos - 1) = atoi(argv[i]);
		}
		
		if (crea_sala(datos_sala[0]) == -1) {
			perror("No se pudo crear la sala");
			exit(1);
		}
		if (recupera_estado_sala(dir) == -1) {
			elimina_sala();
			perror("No se pudo recuperar el estado de la sala");
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
			exit(1);
		}
		
		elimina_sala();
		exit(0);
	}

	if (!strcmp(option, "anula")) {
		struct option longopts[] = {
			{"asientos", required_argument, NULL, 
		}
	}
	
	if (!strcmp(option, "estado")) {
		int n_asientos;
		
		int opt = getopt(argc, argv, "f:");
		if (opt == -1) {
			perror("Lectura de ruta incorrecta");
			exit(1);
		}
		
		char* dir = optarg;
		int fd = open(dir, O_RDONLY);
		CHECK_ERROR(fd);
		SELECT_DATOS_SALA(fd, 0);
		int accesos = 1;
		int* sala = NULL;
		while (sala == NULL && datos_sala[0]/accesos < 4) {
			sala = malloc(sizeof(int) * (datos_sala[0] / accesos));
			accesos *= 2;
		}
		int accesos_lectura = 0;
		if (accesos == 1) {
			reemplaza_sala(sala, datos_sala[0], datos_sala[1]);
			estado_sala(dir);
		}
		printf("la capacidad de la sala es: %d y tiene %d asientos ocupados\n{ ", datos_sala[0], datos_sala[1]);
		while (accesos > accesos_lectura) {
			ssize_t bytes_leidos = read(fd, sala, sizeof(int) * datos_sala[0] / accesos);
			CHECK_LEIDO(bytes_leidos);
			accesos_lectura += bytes_leidos / sizeof(int);
		}
	}

}
