// reto 3: poner pausas largas para que se vea, mandar vídeo por sharepoint

void* mostrar_estado(void* arg) {

}

void* reserva_anula(void* arg) {
	
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		fprintf(stderr, "Número de argumentos incorrecto. Introduzca \"multihilos n\"\n");
	}

	int n_hilos = atoi(argv[1]);
	
	if (n_hilos <= 0) {
		fprintf(stderr, "El segundo argumento debe ser un entero positivo. Introduzca \"multihilos n\"\n");
	}
	
	pthread_t hilos[n_hilos], estado;
	crea_sala();
	
	for (int i = 0; i < n_hilos; i++) {
		if (pthread_create(&hilos[i], NULL, reserva_anula, NULL) != 0) {
			elimina_sala();
			perror("Error en la creación de los hilos");
			exit(1);
		}
	}
	
	if (pthread_create(&estado, NULL, mostrar_estado, NULL) != 0) {
		elimina_sala();
		perror("Error en la creación de los hilos");
		exit(1);
	}
	
	for (int i = 0; i < n_hilos; i++) {
		pthread_join(hilos[i], NULL);
	}
}
