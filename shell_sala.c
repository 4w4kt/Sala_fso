#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sala.h"
#define MAX_COMMAND_LEN 100

void printHelp() {
    puts("================ GUÍA DE USO ================");
    puts("- reserva <id-persona>: reserva un asiento para la persona con ese id");
    puts("- libera <id-persona>: libera el asiento ocupado por la persona con ese id");
    puts("- estado_asiento <id-asiento>: informa sobre el estado del asiento con ese id");
    puts("- asientos_libres: indica cuántos asientos libres hay en la sala");
    puts("- asientos_ocupados: indica cuántos asientos ocupados hay en la sala");
    puts("- capacidad_sala: indica la capacidad de la sala");
    puts("- reserva_multiple <id1> <id2> ... <idn>: reserva, si es posible, ");
    puts( "                                          un asiento para todas las personas indicadas");
    puts("- estado_sala: informa sobre el estado de la sala");
    puts("- cerrar_sala: elimina esta sala");
    puts("=============================================");
}


int main(int argc, char* argv[]) {

    char command[MAX_COMMAND_LEN];

    printf("Biaenvenido a la sala de teatro %s.\nSu capacidad es de %s.\n", argv[1], argv[2]);
    crea_sala(atoi(argv[2]));

    while(1) {

        printf("Sala %s>> ", argv[1]);

        fgets(command, MAX_COMMAND_LEN, stdin);

        command[strlen(command) - 1] = '\0';

        char* arg1 = strtok(command, " ");
        char* arg2 = strtok(NULL, "");
        
        if (arg1 == NULL) continue;

        if (!strcmp(arg1, "reserva") && arg2 != NULL) {
            int bookStatus = reserva_asiento(atoi(arg2));
            if (bookStatus == -1) puts("No pudo reservar el asiento.");
            else puts("Se reservó el asiento.");
            continue;
        }

        if (!strcmp(arg1, "libera") && arg2 != NULL) {
            int freeStatus = levantarse(atoi(arg2));
            if (freeStatus == -1) puts("No se pudo liberar el asiento.");
            else puts("Se liberó el asiento");
            continue;
        }

        if (!strcmp(arg1, "estado_asiento") && arg2 != NULL) {
            int seatStatus = estado_asiento(atoi(arg2));
            if (seatStatus == 0) puts("El asiento está libre;");
            else if (seatStatus == -1) puts ("Se produjo un error.");
            else printf("El asiento está ocupado por %d.", seatStatus);
            continue;
        }

        if (!strcmp(arg1, "asientos_libres")) {
            int freeSeats = asientos_libres();
            if (freeSeats == -1) puts("Se produjo un error.");
            else printf("Hay %d asientos libres.\n", freeSeats);
            continue;
        }

        if (!strcmp(arg1, "asientos_ocupados")) {
            int occSeats = asientos_ocupados();
            if (occSeats == -1) puts("Se produjo un error.");
            else printf("Hay %d asientos ocupados.\n", occSeats);
            continue;
        }
        
        if (!strcmp(arg1, "capacidad_sala")) {
        	int capSala = capacidad_sala();
        	if (capSala == -1) puts ("Se produjo un error.");
        	else printf("La capacidad de la sala es %d.\n", capSala);
        	continue;
        }

        if (!strcmp(arg1, "reserva_multiple")) {
            int nSeats = 0;
            int seats[capacidad_sala()];
			char* arg3 = strtok(arg2, " ");
            while (arg3 != NULL) {
                seats[nSeats++] = atoi(arg3);
                arg3 = strtok(NULL, " ");
            }

            int multBook = reserva_multiple(nSeats, seats);

            if (multBook == -1 && !DETALLES)  puts("No se pudo realizar la reserva.");
            else if(multBook > 0 && !DETALLES)  printf("Se reservaron %d asientos.\n", multBook);

            continue;
        }

        if (!strcmp(arg1, "estado_sala")) {
            estado_sala("Estado de la sala:");
            continue;
        }

        if (!strcmp(arg1, "cerrar_sala")) {
        	int retVal = asientos_libres();
            elimina_sala();
            exit(retVal != 0);
        }

        if (!strcmp(arg1, "help")) {
            printHelp();
            continue;
        }

        puts("No se ha reconocido el comando. Escriba \"help\" para leer la guía de uso.");
    }
}
