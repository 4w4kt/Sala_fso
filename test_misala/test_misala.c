
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include "sala.h"

#define COMPROBAR_SALA_VACIA(x)\
    DEBESERCIERTO(capacidad_sala() == 20); \
    DEBESERCIERTO(asientos_libres() == 20); \
    DEBESERCIERTO(asientos_ocupados() == 0); \
    for(int i = 1; i <= 20; i++){\
        DEBESERCIERTO(estado_asiento(i) == 0);\
    }


int crea_test_sh(char* metodo){
    pid_t pid_creado = fork();
    if (pid_creado == -1) {
        perror("Se produjo un error al crear la sucursal.");
        exit(1);
    }
    if (pid_creado == 0) {
        int ret = execlp("sh", "sh", "-c", metodo, NULL);
        if (ret == -1) {
            perror("No se pudo crear una nueva sala.");
            exit(1);
        }

    }
    int status;
    pid_t pidhijo = wait(&status);
    return WEXITSTATUS(status);
    }

    int setup(){
        int fd = open(sala_vacia.txt, O_RDWR|O_CREAT|O_TRUNC);
        CHECK_ERROR (fd);
        close(fd);
        int fd = open(sala_vacia_copia.txt, O_RDWR|O_CREAT|O_TRUNC);
        CHECK_ERROR (fd);
        close(fd);
        fd = open(sala_creada.txt, O_RDWR|O_CREAT|O_TRUNC);
        CHECK_ERROR (fd);
        crea_sala(20);
        int* ids [20];
        for(int i = 1; i <= 20; i++){
            ids[i] = i*3;
        }
        reserva_multiple(20, ids);
        levantarse_asiento(3);
        levantarse_asiento(9);
        guarda_estado_sala(sala_creada.txt);
        close(fd);
        return 0;

        fd = open("sin_permisos.txt", O_CREAT | O_WRONLY | O_TRUNC, 0000);
        if (fd == -1) {
            perror("Error creando el archivo");
            return 1;
        }
        close(fd);    
    }

void test_practia_3(){
    if(DETALLES){
        printf("Vamos a testear las funciones de la practica 3\n");
        printf("Vamos a guardar una sala de 20 elementos en sala.c\n");
    }
    crea_sala(20);
    if(DETALLES) printf("Vamos a guardar una sala de 20 elementos en sala.c\n");
    guarda_estado_sala(sala_1.txt);
    if(DETALLES) printf("Vamos a cargar la sala de 20 elementos\n");
    recupera_estado_sala(sala_1.txt);
    COMPROBAR_SALA_VACIA(20);
    if(DETALLES) printf("Vamos a reservar 10 asientos con el id 1\n");
    for(int i = 1; i <= 10; i++){
        reserva_asiento(1);
    }
    if(DETALLES) printf("Vamos a guardar la sala en el fichero sala_2.txt\n");
    guarda_estado_sala(sala_2.txt);
    if(DETALLES) printf("Vamos a cargar la sala de 20 elementos\n");
    recupera_estado_sala(sala_2.txt);
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == 10);
    DEBESERCIERTO(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++){
        DEBESERCIERTO(estado_asiento(i) == 1);
    }
    for(int i = 11; i <= 20; i++){
        DEBESERCIERTO(estado_asiento(i) == 0);
    }
    if(DETALLES) printf("Vamos a reservar los siguientes 10 asientos co ids 2\n");
    for(int i = 11; i <= 20; i++){
        reserva_asiento(2);
    }
    if(DETALLES)printf("vamos a guardar los pirmeros 10 asientos en sala_2.txt que estaba vacía");
    int id_asientos1[10];
    for (int i =1; i <= 10; i++){
        id_asientos1[i] = i;
    }
    if(DETALLES){
        estado_sala("Estado de la sala actual");
        printf("Ahora vamos a guardar los el estado de los 10 ultimos asietos en sala_3.txt\n");
        printf("y vamos a guardar toda la sala en sala_4.txt\n");
        printf("de tal forma que sala_1.txt y sala_2.txt tiene que tener los primeros 10 asientos reservados por el id 1\n");
        printf("sala_3.txt tiene los ultimos 10 asientos reservados por el id 2\n");
        printf("y sala_4.txt tiene la sala actual\n");
        }
    guardar_estado_parcial_sala(sala_1.txt, 10, id_asientos1);
    guarda_estado_sala(sala_4.txt);
    int id_asientos2[10];
    for (int i = 10; i <= 20; i++){
        id_asientos[i] = i;
    }
    guarda_estado_parcial_sala(sala_3.txt, 10, id_asientos2);
    if(DETALLES) printf("Ahora vamos a comprobar que la sala_1.txt tiene los primeros 10 asientos reservados por el id 1\n");
    recupera_estado_sala(sala_1.txt);
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == 10);
    DEBESERCIERTO(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++){
        DEBESERCIERTO(estado_asiento(i) == 1);
    }
    for(int i = 11; i <= 20; i++){
        DEBESERCIERTO(estado_asiento(i) == 0);
    }
    if(DETALLES){
        estado_sala("sala_1.txt");
        printf("Ahora vamos a comprobar que la sala_2.txt tiene los pr 10 asientos reservados por el id 2\n");
    }
    recupera_estado_sala(sala_2.txt);
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == 10);
    DEBESERCIERTO(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++){
        DEBESERCIERTO(estado_asiento(i) == 1);
    }
    for(int i = 11; i <= 20; i++){
        DEBESERCIERTO(estado_asiento(i) == 0);
    }
    if(DETALLES) {
        estado_sala("sala_2.txt");
        printf("Ahora vamos a comprobar que la sala_3.txt tiene los ultimos 10 asientos reservados por el id 2\n");
    }
    crea_sala(20);
    recupera_estado_sala(sala_3.txt);
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == 10);
    DEBESERCIERTO(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++){
        DEBESERCIERTO(estado_asiento(i) == 0);
    }
    for(int i = 11; i <= 20; i++){
        DEBESERCIERTO(estado_asiento(i) == 2);
    }
    if(DETALLES){
        estado_sala("sala3.txt");
        printf("Ahora vamos a comprobar que la sala_4.txt tiene la sala actual\n");
    }
    recupera_estado_sala(sala_4.txt);
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == 0);
    DEBESERCIERTO(asientos_ocupados() == 20);
    for(int i = 1; i <= 10; i++){
        DEBESERCIERTO(estado_asiento(i) == 1);
    }
    for(int i = 11; i <= 20; i++){
        DEBESERCIERTO(estado_asiento(i) == 2);
    }
    if(DETALLES) estado_sala("sala_4.txt");
}


// que pasa cuando recuperas una sala con asienntos no guardados...
test_casos_peculiares_2(){
    crea_sala(20);
    int ids[10];
    for(int i = 1; i <= 11; i++){
        ids[i-1] = i;
    }
    DEBESERCIERTO(recupera_estado_parcial_sala(sinpermiso.txt, 10, ids), -1);
    DEBESERCIERTO(recupera_estado_parcial_sala(noexiste.txt, 10, ids), -1);
    DEBESERCIERTO(recupera_estado_parcial_sala(sala_vacia.txt, 10, ids), -1);

    if(DETALLES){
        printf("Hemos tratado de abrir recuperar salas en ficheros que no existen\n");
        printf("ficheros vacios \n");
    }
}





int main(){
    setup();
    test_crea();
    test_reserva();

}
