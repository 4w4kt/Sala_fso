
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include "sala.h"

#define COMPROBAR_SALA_VACIA(x)\
    DEBESERCIERTO(capacidad_sala() == x); \
    DEBESERCIERTO(asientos_libres() == x); \
    DEBESERCIERTO(asientos_ocupados() == 0); \
    for(int i = 1; i <= x; i++){\
        DEBESERCIERTO(estado_asiento(i) == 0);\
    }


char fichero_vacio[] = "./test_misala/fichero_vacio.txt";
char sala_creada[] = "./test_misala/sala_creada.txt";
char sin_permisos[] = "./test_misala/sin_permisos.txt";
char no_existe[] = "./test_misala/no_existe.txt";
char sala_1[] = "./test_misala/sala_1.txt";
char sala_2[] = "./test_misala/sala_2.txt";
char sala_3[] = "./test_misala/sala_3.txt";
char sala_4[] = "./test_misala/sala_4.txt";
char sala_97[] = "./test_misala/sala_97.txt";
char sala_37[] = "./test_misala/sala_37.txt";

int setup(){
    int fd = open(fichero_vacio, O_RDWR|O_CREAT|O_TRUNC);
    CHECK_ERROR (fd);
    close(fd);
    crea_sala(20);
    int ids [] ={5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
    reserva_multiple(20, ids);
    levantarse_asiento(3);
    levantarse_asiento(9);
    guarda_estado_sala(sala_creada);
    return 0;

    fd = open(sin_permisos, O_CREAT | O_WRONLY | O_TRUNC, 0000);
    CHECK_ERROR (fd);
    close(fd); 
    elimina_sala();   
}


void test_general(){
    if(DETALLES){
        printf("Vamos a testear las funciones de la practica 3\n");
        printf("Vamos a guardar una sala de 20 elementos en sala.c\n");
    }
    crea_sala(20);
    if(DETALLES) printf("Vamos a guardar una sala de 20 elementos en sala_1.c\n");
    guarda_estado_sala(sala_1);
    if(DETALLES) printf("Vamos a cargar la sala de 20 elementos\n");
    recupera_estado_sala(sala_1);
    COMPROBAR_SALA_VACIA(20);

    if(DETALLES) printf("Vamos a reservar 10 asientos con el id 1\n");
    for(int i = 1; i <= 10; i++) reserva_asiento(1);

    if(DETALLES) printf("Vamos a guardar la sala en el fichero sala_2.txt\n");
    guarda_estado_sala(sala_2);

    if(DETALLES) printf("Vamos a cargar la sala de sala_2.txt\n");
    recupera_estado_sala(sala_2);
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == 10);
    DEBESERCIERTO(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++) DEBESERCIERTO(estado_asiento(i) == 1);
    for(int i = 11; i <= 20; i++) DEBESERCIERTO(estado_asiento(i) == 0);

    if(DETALLES) printf("Vamos a reservar los últimos 10 asientos con ids 2\n");
    for(int i = 11; i <= 20; i++) reserva_asiento(2);
    
    if(DETALLES)printf("vamos a guardar los pirmeros 10 asientos en sala_1.txt que estaba vacía");
    int id_asientos1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    guardar_estado_parcial_sala(sala_1, 10, id_asientos1);
    if(DETALLES){
        estado_sala("Estado de la sala actual");
        printf("Ahora vamos a guardar los el estado de los 10 ultimos asietos en sala_3.txt\n");
        printf("y vamos a guardar toda la sala en sala_4.txt\n");
        printf("de tal forma que sala_1.txt y sala_2.txt tiene que tener los primeros 10 asientos reservados por el id 1\n");
        printf("sala_3.txt tiene los ultimos 10 asientos reservados por el id 2\n");
        printf("y sala_4.txt tiene la sala actual\n");
        }
    guarda_estado_sala(sala_4);
    int id_asientos2[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    guarda_estado_parcial_sala(sala_3, 10, id_asientos2);
    if(DETALLES) printf("Ahora vamos a comprobar que la sala_1.txt tiene los primeros 10 asientos reservados por el id 1\n");
    recupera_estado_sala(sala_1);
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == 10);
    DEBESERCIERTO(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++) DEBESERCIERTO(estado_asiento(i) == 1);
    for(int i = 11; i <= 20; i++) DEBESERCIERTO(estado_asiento(i) == 0);
    if(DETALLES){
        estado_sala("sala_1.txt");
        printf("Ahora vamos a comprobar que la sala_2.txt tiene los primeros 10 asientos reservados por el id 1\n");
    }
    recupera_estado_sala(sala_2);
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == 10);
    DEBESERCIERTO(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++) DEBESERCIERTO(estado_asiento(i) == 1);
    for(int i = 11; i <= 20; i++) DEBESERCIERTO(estado_asiento(i) == 0);
    if(DETALLES) {
        estado_sala("sala_2.txt");
        printf("Ahora vamos a comprobar que la sala_3.txt tiene los ultimos 10 asientos reservados por el id 2\n");
    }
    crea_sala(20);
    recupera_estado_sala(sala_3);
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++) DEBESERCIERTO(estado_asiento(i) == 0);
    for(int i = 11; i <= 20; i++) DEBESERCIERTO(estado_asiento(i) == 2);
    if(DETALLES){
        estado_sala("sala_3.txt");
        printf("Ahora vamos a comprobar que la sala_4.txt tiene la sala actual\n");
    }
    recupera_estado_sala(sala_4);
    DEBESERCIERTO(capacidad_sala() == asientos_ocupados() == 20);
    DEBESERCIERTO(asientos_libres() == 0);
    for(int i = 1; i <= 10; i++) DEBESERCIERTO(estado_asiento(i) == 1);
    for(int i = 11; i <= 20; i++) DEBESERCIERTO(estado_asiento(i) == 2);
    if(DETALLES) estado_sala("sala_4.txt");
}


// que pasa cuando recuperas una sala con asienntos no guardados...
test_recupera_sala_parcial(){
    crea_sala(20);
    int ids_reserva[] = {97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116};
    reserva_multiple(20, ids_reserva);
    int ids[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    DEBESERCIERTO(recupera_estado_parcial_sala(sin_permiso, 10, ids)== -1);
    DEBESERCIERTO(recupera_estado_parcial_sala(no_existe, 10, ids) == -1);
    DEBESERCIERTO(recupera_estado_parcial_sala(sala_vacia, 10, ids) == -1);

    if(DETALLES){
        printf("Hemos tratado de abrir recuperar salas en ficheros que no existen\n");
        printf("ficheros sin permisos y ficheros vacíos \n");
        printf("la sala la habiamos iniciado con todos los asientos con id 97 + Nº de asiento\n");
        printf("y ahora vamos a comprobar que no se han modificado\n");
        estado_sala("sala_recuperada.txt");
    }

    for(int i = 1; i <= 20; i++) DEBESERCIERTO(estado_asiento(i) == i + 97);
    DEBESERCIERTO(capacidad_sala() == asientos_ocupados() == 20);
    DEBESERCIERTO(asientos_libres() == 0);
    if(DETALLES){
        printf("Ahora vamos a guardar la sala actual en sala_recuperada.txt\n");
        printf("y vamos a recuperar los asientos pares de la sala actual desde el fichero sala_97.txt\n");
        printf("y vamos a comprobar que se ha recuperado correctamente\n");
    }
    guarda_estado_sala(sala_97);
    crea_sala(20);
    if(DETALLES) estado_sala("Antes de recuperar la sala");
    int ids_pares [] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
    
    recupera_estado_parcial_sala(sala_97, 10, ids_pares);
    if(DETALLES) estado_sala("Después de recuperar los asientos pares");
    DEBESERCIERTO(capacidad_sala() == 20);
    DEBESERCIERTO(asientos_libres() == asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++){
        DEBESERCIERTO(estado_asiento(ids_pares[i]) == ids_pares[i] + 98);
    }
    if(DETALLES){
        printf("Ahora vamos a tratar de recuperar la  sala con argumentos incurrectos\n");
    }
    ids_reserva[] = {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49};
    recupera_estado_parcial_sala(sala_97, 10, ids_reserva);
    int ids_impares[10] = {3, 5, 7, 9, 11, 13, 19, 17, 19, 1};
    DEBESERCIERTO(recupera_estado_parcial_sala(sala_97, -10, ids_impares) == -1);
    if(DETALLES){
        printf("Ahora vamos a recuperar los asientos impares, es decir lo que queda para recuperar la sala completa\n");
    }
    recupera_estado_parcial_sala(sala_97, 10, ids_impares);
    if(DETALLES) estado_sala("Despues de recuperar los asientos impares");
    DEBESERCIERTO(capacidad_sala() == asientos_ocupados() == 20);
    DEBESERCIERTO(asientos_libres() == 0);
    for(int i = 1; i <= 20; i++) DEBESERCIERTO(estado_asiento(i) == i + 97);

    if(DETALLES) estado_sala("ahora vamos a tratar de recuperar asientos con una sala de capacidad errónea");
    crea_sala(10)
    DEBESERCIERTO(recupera_estado_parcial_sala(sala_97, 10, ids) == -1);
    liberar_sala();    
}

void test_guarda_sala(){
    DEBESERCIERTO(guardar_sala("sala_guardada.txt") == -1);
    if(DETALLES){
        printf("comprobamos que no se puede guardar salas que no existen\n");
        printf("ni salas en ficheros que no existen o no tenemos permisos\n");
    }
    DEBESERCIERTO(guardar_sala(no_existe) == -1);
    DEBESERCIERTO(guardar_sala(sin_permiso) == -1);    
    
    crear_sala(20);
    DEBESERCIERTO(guardar_sala("sala_vacia_20.txt") == 0);
    DEBESERCIERTO(guardar_sala(sin_permiso) == -1);
    if(DETALLES) printf("Ahora vamos a guardar una sala vacía de 20 asientos en sala_vacia_20.txt\n");
    recupera_estado_sala("sala_vacia_20.txt");
    if(DETALLES) estado_sala("sala_vacia_20.txt");
    COMPROBAR_SALA_VACIA("sala_vacia_20.txt");
    if(DETALLES) printf("Ahora vamos intentar a guardar una sala vacía de 10 asientos en sala_vacia_20.txt\n");
    crea_sala(10);
    DEBESERCIERTO(guardar_sala("sala_vacia_20.txt") == -1);
    if(DETALLES) printf("Ahora vamos intentar a guardar una sala llena de 10 asientos en sala_llena_10.txt\n");
    for(int i = 1; i <= 10; i++){
        reserva_asiento(i*3);
    }
    DEBESERCIERTO(guardar_sala("sala_llena_10.txt") == 0);
    if(DETALLES) estado_sala("sala_llena_10.txt");
    crea_sala(10);
    recupera_estado_sala("sala_llena_10.txt");
    for (int i = 1; i <= 10; i++){
        DEBESERCIERTO(estado_asiento(i) == i*3);
    }
    DEBESERCIERTO(capacidad_sala() == asientos_ocupados() == 10);
    DEBESERCIERTO(asientos_libres() == 0);
    }


    void test_recupera_sala(){
        
    crea_sala(20);
    int ids_reserva[20] = {38 , 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
    reserva_multiple(20, ids_reserva);
    int ids[10];
    for(int i = 1; i <= 11; i++) ids[i-1] = i;
    DEBESERCIERTO(recupera_estado_sala(sin_permiso)== -1);
    DEBESERCIERTO(recupera_estado_sala(no_existe) == -1);
    DEBESERCIERTO(recupera_estado_sala(sala_vacia) == -1);

    if(DETALLES){
        printf("Hemos tratado de recuperar salas en ficheros que no existen\n");
        printf("ficheros sin permisos y ficheros vacíos \n");
        printf("la sala la habiamos iniciado con todos los asientos con id 37 + Nº de asiento\n");
        printf("y ahora vamos a comprobar que no se han modificado\n");
        estado_sala("sala_recuperada.txt");
    }

    for(int i = 1; i <= 20; i++) DEBESERCIERTO(estado_asiento(i) == i + 38);
    
    DEBESERCIERTO(capacidad_sala() == asientos_ocupados() == 20);
    DEBESERCIERTO(asientos_libres() == 0);
    if(DETALLES) printf("Ahora vamos a guardar la sala actual en sala_37.txt\n");
    guarda_estado_sala(sala_37);
    crea_sala(20);

    if(DETALLES) estado_sala("Antes de recuperar la sala");
    
    int ids_pares [] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
    recupera_estado_sala(sala_37);
    if(DETALLES) estado_sala("Despues de recuperar la sala");
    DEBESERCIERTO(capacidad_sala() == asientos_ocupados() == 20);
    DEBESERCIERTO(asientos_libres() == 0);
    for(int i = 1; i <= 20; i++){
        DEBESERCIERTO(estado_asiento(i) == i + 37);
        }
    if(DETALLES) estado_sala("ahora vamos a tratar de recuperar salas con distintas capacidades a nuestra sala");
    crea_sala(10);
    DEBESERCIERTO(recupera_estado_sala(sala_37) == -1);
}


int main(){
    setup();
    test_crea();
    test_reserva();

}
