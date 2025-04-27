#include <unistd.h>

int main(int argc, int argv) {

	if (argc < 4) {
		perror("Número de argumentos incorrecto");
		exit(1);
	}

	char* option = argv[1];
	
	if (!strcmp(option, "crea") {
		int opt;
		char* dir;
		int override = 0;
		int cap;
		
		while (opt = getopt(argc, argv, "f:c:") != -1) {
		
			if (opt == 'f') {
				if (optarg == "o") {
					override = 1;
					break;
				}
				
				dir = optarg;
				break;
			}
			
			if (opt == 'c') {
				cap = atoi(optarg);
				break;
			}
			
		}
		
		if (override) {
		
			while (opt = getopt(argc, argv, "f:c:") != -1) {
			
				if (opt == 'o') {
					dir = optarg;
					break;
				}	
			}
		}
		
		// Gestionar archivo (CHEQUEAR LO DE MARCI EN DC)
		if (override) {
			if (!access(dir, F_OK) && access(dir, W_OK)) {
				perror("No tiene permisos de escritura para la ruta indicada");
				exit(1);
			}
			if (crea_sala(cap) == -1) {
				perror("No se pudo crear la sala");
				exit(1);
			}
			if (guarda_estado_sala(dir) == -1) {
				perror("No se pudo guardar el estado de la sala en la ruta indicada");
				exit(1);
			}
			exit(0);
		}
		
		if (!access(dir, F_OK)) {
			perror("La ruta ya existe pero no ha indicado la opción de sobreescritura");
			exit(1);
		}
		if (crea_sala(cap) == -1) {
				perror("No se pudo crear la sala");
				exit(1);
			}
		if (guarda_estado_sala(dir) == -1) {
			perror("No se pudo guardar el estado de la sala en la ruta indicada");
			exit(1);
	
	}
	
	if (!strcmp(option, "reserva") {
	
	}

	if (!strcmp(option, "anula") {
	
	}
	
	if (!strcmp(option, "estado") {
	
	}

}
