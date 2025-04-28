
#include "sala.h"
#include <stdlib.h>

#define preparahijo
int test_crea(){
    pid_t pid_hijo = fork();
 
}

void crea_sucursal(char* ciudad, char* capacidad){

    pid_t pid_creado = fork();
    char* saveback = ciudad;

    if (pid_creado == -1) {
        perror("Se produjo un error al crear la sucursal.");
        exit(1);
    }

    if (pid_creado == 0) {

        if (setpgid(0, 0) == -1){
        perror("setpgid");
        exit(1);
        }

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
        }    free(nombre_sucursal);
            nombre_sucursal = NULL;
            printf("Gestor de sucursales>> ");
            fflush(stdout);
    }
}










int main(){
    test_crea();
}