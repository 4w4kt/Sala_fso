#include <unistd.h>
#include <getopt.h>

int main(int argc, int argv) {

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
		
		dir = optarg;
		
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
			{"asientos", required_argument, NULL, 'a'},
			{"personas", required_argument, NULL, 'p'},
			{0, 0, 0, 0}
		}
		
		int asientos = 0;
		
		while (opt = getopt_long_only(argc, argv, "f:", longopts, NULL) != -1) {
		        if (opt == 'f') {
		                dir = optarg;
		        }
		        if (opt == 'a' && asientos >= 0) {
		                asientos = ;
		        }
		}
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
		int* sala = NULL;
		while (sala == NULL) {
			sala = malloc(sizeof(int) * (datos_sala[0] / accesos));
			accesos *= 2;
		}
		accesos /= 2;
		if (accesos == 1) {
			reemplaza_sala(sala, datos_sala[0], datos_sala[1]);
			estado_sala(dir);
			libera_sala(sala);
			close(fd);
			exit(0);
		}
			
		// falta comprobar el caso de que no se pueda cargar la sala
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
		}



	if(!strcmp(option, "compara")) {
		if(argc != 3){
			perror("Se esperaban dos argumentos ejemplo: ./misala compara sala1.txt sala2.txt");
			exit(1);
		}
		if(!strcomp(argv[1], argv[2])){
			return 0;
		}
		
		int fd1 = open(argv[2], O_RDONLY);
		CHECK_ERROR(fd1);
		int fd2 = open(argv[3], O_RDONLY);
		CHECK_ERROR(fd2);
		int datos_sala_1[2];
		int datos_sala_2[2];
		ssize_t bytes_leidos = read(fd1, &datos_sala_1, sizeof(int)*2);
		CKECK_LEIDO(bytes_leidos); 
		ssize_t bytes_leidos2 = read(fd2, &datos_sala_2, sizeof(int)*2);
		CKECK_LEIDO(bytes_leidos2);
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
		if(sala_1 == -1  || sala_2 == -1){
			perror("Error al asignar memoria para las dos salas");
			exit(1);
		}
		bytes_leidos = 1;
		while (bytes_leidos > 0){
			ssize_t bytes_leidos = read(fd1, sala_1, sizeof(int) * (datos_sala_1[0] / accesos));
			CHECK_LEIDO(bytes_leidos);
			ssize_t bytes_leidos2 = read(fd2, sala_2, sizeof(int) * (datos_sala_2[0] / accesos));
			CHECK_LEIDO(bytes_leidos2);
			for (int i = 0; i < bytes_leidos2*sizeof(int); i++) {
				if (*(sala_1 + i) != *(sala_2 + i)) {
					printf("Las salas no son iguales\n");
					close(fd1);
					close(fd2);
					libera_sala(sala_1);
					libera_sala(sala_2);
					return 1;
					exit(1);
				}
			}
		}
		return 0;
	}

}
