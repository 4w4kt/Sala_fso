#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>	
#include <error.h>

#include <ctype.h>
#include <poll.h>
#include <string.h>

#ifndef programa_principal			
#define MAX_COMMAND_LEN 100
#define MAX_SUCURSALES 20
#endif

#define END_COMMAND 	printf("Gestor de sucursales>> "); fflush(stdout); continue; 

struct Sucursal {
	pid_t pid_proceso;
	char* nombre;
};

int hijos_vigentes = 0;
struct Sucursal sucursales[MAX_SUCURSALES];
int status;


void printHelp() {
    puts("================ GUÍA DE USO ================");
    puts("- crear_sucursal <nombre> <capacidad>: reserva un asiento para la persona con ese id");
    puts("- cerrar_gestor: cierra el gestor de sucursales");
    puts("- ver_salas: muestra los nombres y PIDs de las salas vigentes");
    puts("=============================================");
}
char* check_num(char* num){
	if(num == NULL){
		printf("El comando no se introdujo correctamente.\n"); 
		return 0;
	}
	num = strchr(num, ' ');
	if(num == NULL){
		printf("El comando no se introdujo correctamente.\n"); 
		return 0;
	}
	num[0] = '\0';
	num++;
	char* next = strchr(num, ' ');
	if(num == NULL || next != NULL){
		printf("El comando no se introdujo correctamente.\n"); 
		return 0;
	}
	if (atoi(num) <= 0) {
		puts("La cantidad tiene que ser un número mayor que 0."); 
		return 0;
	}
	return num;
}
char* extraer_datos(char* nombre, char** result){
	char* saveback = nombre;
	char* num = NULL; 
 	int cantidad = 0;
	nombre = strchr(nombre, '"');
	
	if (nombre) {
		nombre[0] ='\0';
		nombre++;
		nombre = strtok(nombre, "\"");
		if (nombre) {
			num = strtok(NULL, "");
			*result = check_num(num);
			return nombre;
		}
		return 0;
	}
	
	nombre = saveback;
	nombre = strchr(nombre, '\'');
	
	if (nombre) {
		nombre[0] ='\0';
		nombre++;
		nombre = strtok(nombre, "\'");
		if (nombre) {
			num = strtok(NULL, "");
			*result = check_num(num);
			return nombre;
		}
		return 0;
	}
	
	nombre = strtok(saveback, " ");
	num = strtok(NULL, "");
	if (num == NULL || atoi(num) <= 0 || strchr(num, ' ') != NULL) {
		puts("Comando incorrecto"); 
		return 0;
	}
	*result = num;
	return nombre;	
}

int buscar_libre() {
	if (hijos_vigentes == MAX_SUCURSALES) return -1;

	struct Sucursal sucursal;
	
	for (int i = 0; i < MAX_SUCURSALES; i++) {
		sucursal = sucursales[i];
	
		if (sucursal.pid_proceso <= 0) {
			hijos_vigentes++;
			return i;
		}
	}
	return -1;
}

char* encontrar_sucursal(pid_t pid_sucursal) {
	
	struct Sucursal sucursal;
	for (int i = 0; i < MAX_SUCURSALES; i++) {

		sucursal = sucursales[i];
		if (sucursal.pid_proceso == pid_sucursal) {
			char* nombre = sucursal.nombre;
			sucursal.pid_proceso = -1;
			hijos_vigentes--;
			return nombre;
		}
	}
	return NULL;
}

void ver_salas(){

	struct Sucursal sucursal;
	if (hijos_vigentes == 0) {
		 puts("No hay sucursales activas");
		 return;
	}
	
	int sucursales_contadas = 0;
	printf("{");
	int i = 0;
	while(sucursales_contadas < hijos_vigentes){
		sucursal = sucursales[i++];
		if (sucursal.pid_proceso > 0) {
			printf("{%s, %d}", sucursal.nombre, sucursal.pid_proceso);
			if (hijos_vigentes != ++sucursales_contadas)printf(",");
		}
	}
	puts("}");		
}

void crea_sucursal(char* ciudad, char* capacidad){
	
	pid_t pid_creado = fork();
	char* saveback = ciudad;
	
	if (pid_creado == -1) {
		perror("Se produjo un error al crear la sucursal.");
		exit(1);
	}
	
	if (pid_creado == 0) {
		int ret = execlp("gnome-terminal", "gnome-terminal", "--wait", "--", "./sala", ciudad, capacidad, NULL);
		if (ret == -1) {
			perror("No se pudo crear una nueva sala.");
			exit(1);
		}
	}
	int idx = buscar_libre();
	if (idx == -1) {
		puts("Se ha alcanzado el máximo de sucursales creadas."); 
		return;
	}
	
	char* datos = malloc(strlen(ciudad) +1);
	strcpy(datos, ciudad);
	struct Sucursal nueva_sucursal = {pid_creado, datos};
	sucursales[idx] = nueva_sucursal;
	
}

void vigila_hijo() {
	
	pid_t pidhijo = waitpid(-1, &status, WNOHANG);
	
	if (pidhijo > 0) {
		char* nombre_sucursal = encontrar_sucursal(pidhijo);
		
	
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) == 0) {
				printf("La sucursal %s se cerró con todos los asientos ocupados.\n", nombre_sucursal);
			} else if (WEXITSTATUS(status) == 1) {
				printf("La sucursal %s se cerró con algún asiento libre.\n", nombre_sucursal);
			} else {
				printf("La sucursal %s se cerró sin proporcionar información sobre sus asientos.\n", nombre_sucursal);
			}
		} else {
			printf("La sucursal %s terminó de manera anormal\n", nombre_sucursal);
		}	free(nombre_sucursal);
			nombre_sucursal = NULL;
			printf("Gestor de sucursales>> ");
			fflush(stdout);
	}
}
	
int main(int argc, char* argv[]) {
	
	char command[MAX_COMMAND_LEN];
	int poll_val;
	
	struct pollfd polls[1];
	polls[0].fd = STDIN_FILENO;
	polls[0].events = POLLIN;

    puts("Bienvenido al gestor de sucursales. Introduzca opción:");
	printf("Gestor de sucursales>> ");
	fflush(stdout); 
		
	while (1) {
		vigila_hijo();
	    poll_val = poll(polls, 1, 0);
	    
	    if (poll_val == -1) {
	    
	    	perror("poll() generó un error");
	    	exit(1);
	    	
	    } else if (poll_val > 0) {

			fgets(command, MAX_COMMAND_LEN, stdin);
			command[strlen(command) - 1] = '\0';
			char* arg1 = strtok(command, " ");
			
			if (arg1 == NULL) {
				END_COMMAND;
			}
			
			if (!strcmp(arg1, "help")) {
			    printHelp();
			    END_COMMAND;
			}
			
			if (!strcmp(arg1, "cerrar_gestor")) {
			    exit(0);
			}
			
			
			if (!strcmp(arg1, "crear_sucursal")) {
				char* nombre = strtok(NULL, "");
				char* capacidad;
				
				if(nombre) nombre = extraer_datos(nombre, &capacidad);
				else puts("Faltan parámetros para crear la sucursal. Escriba \"help\" para leer la guía de uso.");
				if (nombre == 0){END_COMMAND}
			    if (capacidad == 0){
			    	puts("La capacidad tiene que ser un número mayor que 0. Escriba \"help\" para leer la guía de uso.");
			    	END_COMMAND
			    }
			    crea_sucursal(nombre, capacidad);
				printf("==========\nSe ha creado la sala %s\ncon capacidad %s \n==========\n", nombre, capacidad);	 
			    END_COMMAND;
			}
			
			if (!strcmp(arg1, "ver_salas")) {
			    ver_salas();
			    END_COMMAND;
			}
		   

			puts("No se ha reconocido el comando. Escriba \"help\" para leer la guía de uso.");
			END_COMMAND;
		}
	}
}


