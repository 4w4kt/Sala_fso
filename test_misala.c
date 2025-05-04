#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/wait.h>
#include <assert.h>

#include "sala.h"
#include "macros.h"

#define COMPROBAR_SALA_VACIA(x)\
    DebeSerCierto(capacidad_sala() == x); \
    DebeSerCierto(asientos_libres() == x); \
    DebeSerCierto(asientos_ocupados() == 0); \
    for(int i = 1; i <= x; i++){\
        DebeSerCierto(estado_asiento(i) == 0);\
    }
#define REMOVE(x) if(remove(x) != 0) perror("Error al eliminar el archivo");


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
char sala_vacia_20[] = "./test_misala/sala_vacia_20.txt";
char sala_llena_10[] = "./test_misala/sala_llena_10.txt";



int ids_completos[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
int ids_primeros[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int ids_ultimos[] = {20, 19, 18, 17, 16, 15, 14, 13, 12, 11};
int ids_pares[] = {2, 2, 2, 4, 6, 8, 14, 16, 18, 10, 12, 20};
int ids_impares[] = {1, 1, 3, 3, 9, 11, 13, 5, 7, 15, 17, 19};
int ids_5[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100};


int setup(){
    int fd = open(fichero_vacio, O_RDWR|O_CREAT|O_TRUNC);
    CHECK_ERROR (fd);
    close(fd);
    crea_sala(20);
    reserva_multiple(20, ids_5);
    libera_asiento(3);
    libera_asiento(9);
    guarda_estado_sala(sala_creada);

    fd = open(sin_permisos, O_CREAT | O_WRONLY | O_TRUNC);
    fchmod(fd, 0000);
    CHECK_ERROR(fd);
    close(fd); 
    if (chmod(sin_permisos, 0000) != 0) {
        perror("chmod");
        return 1;
    }
    elimina_sala();   
}


void test_general(){

    INICIO_TEST_NOMBRE("Test general");

    if(DETALLES){
        printf("Guardar y cargar una sala de 20 elementos\n");
    }
    crea_sala(20);
    guarda_estado_sala(sala_1);
    recupera_estado_sala(sala_1);
    COMPROBAR_SALA_VACIA(20);
    estado_sala("");

    if(DETALLES) printf("Reservar asientos, guardar y recuperar en sala_2.txt\n");
    for(int i = 1; i <= 10; i++) reserva_asiento(1);
    guarda_estado_sala(sala_2);

    recupera_estado_sala(sala_2);
    DebeSerCierto(capacidad_sala() == 20);
    DebeSerCierto(asientos_libres() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++) DebeSerCierto(estado_asiento(i) == 1);
    for(int i = 11; i <= 20; i++) DebeSerCierto(estado_asiento(i) == 0);
    estado_sala("");

    if(DETALLES) printf("Reservar más asientos, guardarlos en sala_1.txt\n");
    for(int i = 11; i <= 20; i++) reserva_asiento(2);
    guarda_estado_parcial_sala(sala_1, 10, ids_primeros);
    estado_sala("");

    if(DETALLES){
        printf("Guardar últimos 10 asientos en sala_3.txt, toda la sala en sala_4.txt:\n");
        printf("- Sala 1/2 = 10 primeros asientos, id 1\n");
        printf("- Sala 3 = 10 últimos asientos, id 2\n");
        printf("- Sala 4 = sala actual\n");
        }

    guarda_estado_sala(sala_4);
    crea_sala(20);
    guarda_estado_sala(sala_3);
    recupera_estado_sala(sala_4);
    guarda_estado_parcial_sala(sala_3, 10, ids_ultimos);
    
    // Volviendo a comprobar
    recupera_estado_sala(sala_1);
    DebeSerCierto(capacidad_sala() == 20);
    DebeSerCierto(asientos_libres() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++) DebeSerCierto(estado_asiento(i) == 1);
    for(int i = 11; i <= 20; i++) DebeSerCierto(estado_asiento(i) == 0);
    estado_sala("\n=========SALA 1:");
    
    recupera_estado_sala(sala_2);
    DebeSerCierto(capacidad_sala() == 20);
    DebeSerCierto(asientos_libres() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++) DebeSerCierto(estado_asiento(i) == 1);
    for(int i = 11; i <= 20; i++) DebeSerCierto(estado_asiento(i) == 0);
    estado_sala("\n=========SALA 2:");
    
    crea_sala(20);
    recupera_estado_sala(sala_3);
    DebeSerCierto(capacidad_sala() == 20);
    DebeSerCierto(asientos_libres() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++) DebeSerCierto(estado_asiento(i) == 0);
    for(int i = 11; i <= 20; i++) DebeSerCierto(estado_asiento(i) == 2);
    estado_sala("\n=========SALA 3:");
    
    recupera_estado_sala(sala_4);
    DebeSerCierto(capacidad_sala() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 20);
    DebeSerCierto(asientos_libres() == 0);
    for(int i = 1; i <= 10; i++) DebeSerCierto(estado_asiento(i) == 1);
    for(int i = 11; i <= 20; i++) DebeSerCierto(estado_asiento(i) == 2);
    estado_sala("\n=========SALA 4:");

    FIN_TEST("Test general");
}


void test_recupera_sala_parcial(){

    INICIO_TEST_NOMBRE("Test recupera_sala_parcial");

    crea_sala(20);
    int ids_reserva[] = {97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116};
    reserva_multiple(20, ids_reserva);

    if (DETALLES) puts("No se pueden recuperar salas sin permisos, que no existen, vacías.");

    // DebeSerCierto(recupera_estado_parcial_sala(sin_permisos, 10, ids_primeros)== -1); // ABRIR EN MODO USUARIO
    DebeSerCierto(recupera_estado_parcial_sala(no_existe, 10, ids_primeros) == -1);
    REMOVE(no_existe);
    
    DebeSerCierto(recupera_estado_parcial_sala(fichero_vacio, 10, ids_primeros) == -1);

    for(int i = 1; i <= 20; i++) DebeSerCierto(estado_asiento(i) == i + 96);
    DebeSerCierto(capacidad_sala() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 20);
    DebeSerCierto(asientos_libres() == 0);

    if(DETALLES) {
        printf("Guardar y recuperar la sala actual en sala_recuperada.txt;\n");
        printf("Recuperar los asientos pares de la sala actual desde el fichero sala_97.txt\n");
    }
    guarda_estado_sala(sala_97);
    crea_sala(20);
    estado_sala("\n=======Antes de recuperar la sala");

    recupera_estado_parcial_sala(sala_97, 12, ids_pares);
    estado_sala("\n=======Después de recuperar los asientos pares");
    DebeSerCierto(capacidad_sala() == 20);
    DebeSerCierto(asientos_libres() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 10);
    for(int i = 1; i <= 10; i++){
        DebeSerCierto(estado_asiento(i*2) == i*2 + 96);
    }
    if(DETALLES) printf("Argumentos incorrectos:\n");
    for(int i=0; i<20;i++) ids_reserva[i] = i+30;
    recupera_estado_parcial_sala(sala_97, 10, ids_reserva);
    DebeSerCierto(recupera_estado_parcial_sala(sala_97, -10, ids_impares) == -1);
    
    recupera_estado_parcial_sala(sala_97, 12, ids_impares);
    estado_sala("DespuEs de recuperar el resto de la sala");
    DebeSerCierto(capacidad_sala() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 20);
    DebeSerCierto(asientos_libres() == 0);
    for(int i = 1; i <= 20; i++) DebeSerCierto(estado_asiento(i) == i + 96);

    if(DETALLES) estado_sala("Capacidad errónea:");
    crea_sala(10);
    DebeSerCierto(recupera_estado_parcial_sala(sala_97, 10, ids_primeros) == -1);
    elimina_sala();

    FIN_TEST("Test recupera_sala_parcial");
}

void test_guarda_sala(){

    INICIO_TEST_NOMBRE("Test guarda_sala");

    DebeSerCierto(guarda_estado_sala("sala_guardada.txt") == -1);
    if(DETALLES){
        printf("No se guardan salas sin permisos o que no existen:\n");
    }
    DebeSerCierto(guarda_estado_sala(no_existe) == -1);
    // DebeSerCierto(guarda_estado_sala(sin_permisos) == -1);    
    
    crea_sala(20);
    DebeSerCierto(guarda_estado_sala(sala_vacia_20) == 0);
    // DebeSerCierto(guarda_estado_sala(sin_permisos) == -1);
    
    recupera_estado_sala(sala_vacia_20);
    estado_sala("\n=========Sala vacía de 20 asientos");;
    COMPROBAR_SALA_VACIA(20);
    if(DETALLES) printf("Sobreescribir la sala a 10 asientos:\n");
    crea_sala(10);
    DebeSerCierto(guarda_estado_sala(sala_vacia_20) == 0);
    reserva_asiento(1);
    estado_sala("\n=========Sala sobreescrita");
    recupera_estado_sala(sala_vacia_20);
    COMPROBAR_SALA_VACIA(10);

    if(DETALLES) printf("Intentar llenar una sala ya llena:\n");
    for(int i = 1; i <= 10; i++) reserva_asiento(i*3);
    DebeSerCierto(guarda_estado_sala(sala_llena_10) == 0);
    estado_sala("\n=========Sala llena:");
    crea_sala(10);

    estado_sala("\n=========Antes de recuperar la sala (vacía):");

    recupera_estado_sala(sala_llena_10);
    estado_sala("\n=========Después de recuperar la sala (llena)");
    for (int i = 1; i <= 10; i++){
        DebeSerCierto(estado_asiento(i) == i*3);
    }
    DebeSerCierto(capacidad_sala() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 10);
    DebeSerCierto(asientos_libres() == 0);

    FIN_TEST("Test guarda_sala");
    }


void test_recupera_sala() {

    INICIO_TEST_NOMBRE("Test recupera_sala");
        
    crea_sala(20);
    int ids_reserva[20] = {38 , 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
    reserva_multiple(20, ids_reserva);
    //DebeSerCierto(recupera_estado_sala(sin_permisos)== -1);
    //DebeSerCierto(recupera_estado_sala(sin_permisos)== -1);
    DebeSerCierto(recupera_estado_sala(no_existe) == -1);
    DebeSerCierto(recupera_estado_sala(fichero_vacio) == -1);

    if(DETALLES){
        printf("No se pueden recuperar salas sin permisos, inexistentes, vacías\n");;
        estado_sala("\n=========Sala recuperada y modificada");
    }

    for(int i = 1; i <= 20; i++) DebeSerCierto(estado_asiento(i) == i + 37);
    
    DebeSerCierto(capacidad_sala() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 20);
    DebeSerCierto(asientos_libres() == 0);

    if(DETALLES) printf("Guardamos la sala actual en sala_37.txt\n");
    guarda_estado_sala(sala_37);
    crea_sala(20);

    if(DETALLES) estado_sala("Antes de recuperar la sala");
    recupera_estado_sala(sala_37);
    if(DETALLES) estado_sala("Despues de recuperar la sala");
    DebeSerCierto(capacidad_sala() == asientos_ocupados());
    DebeSerCierto(asientos_ocupados() == 20);
    DebeSerCierto(asientos_libres() == 0);
    for(int i = 1; i <= 20; i++) DebeSerCierto(estado_asiento(i) == i + 37);
    if(DETALLES) estado_sala("ahora vamos a tratar de recuperar salas con distintas capacidades a nuestra sala");
    crea_sala(10);
    DebeSerCierto(recupera_estado_sala(sala_37) == -1);
}


void test_guarda_parcial_sala(){
    if(DETALLES) printf("Ahora vamos a intentar guardar parcialmente sin tener sala\n");
    DebeSerCierto(guarda_estado_parcial_sala(sin_permisos, 10, ids_impares) == -1);
    DebeSerCierto(guarda_estado_parcial_sala(no_existe, 10, ids_impares) == -1);
    DebeSerCierto(guarda_estado_parcial_sala(fichero_vacio, 10, ids_impares) == -1);
    DebeSerCierto(guarda_estado_parcial_sala(sala_37, 10, ids_impares) == -1);
    if(DETALLES) printf("Ahora vamos a intentar guardar una sala en ficheros sin permiso\n");
    crea_sala(20);
    DebeSerCierto(guarda_estado_parcial_sala(sin_permisos, 10, ids_impares) == -1);
    DebeSerCierto(guarda_estado_parcial_sala(no_existe, 10, ids_impares) == -1);
    DebeSerCierto(guarda_estado_parcial_sala(fichero_vacio, 10, ids_impares) == -1);
    if(DETALLES) printf("Ahora vamos a guardar los asientos pares de nuestra sala vacía en sala_creada.txt\n");
    guarda_estado_parcial_sala(sala_creada, 12, ids_pares);
    recupera_estado_sala(sala_creada);
    for(int i = 1; i <= 10; i++) DebeSerCierto(estado_asiento(i*2) == 0);
    if(DETALLES) printf("Ahora vamos a guardar los asientos pares el id 141\n");
    while(reserva_asiento(141) != -1);
    guarda_estado_parcial_sala(sala_creada, 12, ids_pares);
    crea_sala(20);
    recupera_estado_sala(sala_creada);
    for(int i = 1; i <= 10; i++) DebeSerCierto(estado_asiento(i*2) == 141);
    for(int i = 1; i <= 10; i++) DebeSerCierto(estado_asiento(i*2 -1) != 141); 
    crea_sala(20);
    reserva_multiple(20, ids_5);
    guarda_estado_parcial_sala(sala_creada, 20, ids_completos);
    crea_sala(20);
    recupera_estado_sala(sala_creada);
    for(int i = 1; i <= 20; i++) DebeSerCierto(estado_asiento(i) == 5*i);    
}


int main(){
    setup();
    test_general();
    test_recupera_sala_parcial();
    test_guarda_sala();
    test_recupera_sala();
    test_guarda_parcial_sala();
}
