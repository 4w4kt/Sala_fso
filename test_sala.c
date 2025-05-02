// test_sala.c
// ===============
// Batería de pruebas de la biblioteca "sala.h/sala.c"
//

#include <assert.h>
#include <stdio.h>
#include "sala.h"


void INICIO_TEST (const char* titulo_test)
{
    printf("\n********** Batería de pruebas para %s:\n", titulo_test);
    // fflush fuerza que se imprima el mensaje anterior
    // sin necesidad de utilizar un salto de línea
    fflush(stdout);
}

void FIN_TEST (const char* titulo_test)
{
    if (DETALLES) printf ("********** Test %s - completado.\n", titulo_test);
    else printf("- Test %s completado\n", titulo_test);
}

void test_ReservaBasica()
{
    int mi_asiento;
    #define CAPACIDAD 500
    #define ID_1 1500

    INICIO_TEST_NOMBRE("Reserva básica");
    crea_sala(CAPACIDAD);
    ESTADO("Al crear la sala");

    DebeSerCierto(capacidad_sala()==CAPACIDAD);
    mi_asiento = reserva_asiento(ID_1);

    if (DETALLES) DebeSerCierto(sentarse(ID_1) >= 0);
    else DebeSerCierto(mi_asiento >= 0);

    ESTADO("Al reservar un asiento");

    DebeSerCierto((asientos_libres() + asientos_ocupados())==CAPACIDAD);

    DebeSerCierto(estado_asiento(mi_asiento)>0);
    if (DETALLES) DebeSerCierto(levantarse(ID_1) == ID_1);
    else DebeSerCierto(libera_asiento(mi_asiento)==ID_1);

    ESTADO("Al liberar el asiento");
    DebeSerCierto((asientos_libres() + asientos_ocupados())==CAPACIDAD);
    elimina_sala();

    ESTADO("Al eliminar la sala");
    FIN_TEST("Reserva básica");
}

void test_capacidad_incorrecta() {
    INICIO_TEST_NOMBRE("Sala con capacidad incorrecta");
    DebeSerCierto(crea_sala(-1) == -1);

    if (DETALLES) puts("La sala no se ha podido crear.");
    FIN_TEST("Sala con capacidad incorrecta");
}

void test_1asiento() {

    INICIO_TEST_NOMBRE("Sala con 1 asiento");
    DebeSerCierto(crea_sala(1) == 1);
    ESTADO("Al crear una sala");
    /// El id de una persona tiene que ser positivo
    DebeSerCierto((reserva_asiento(-20) == -1));

    DebeSerCierto((reserva_asiento(0) == -1));
    /// Le asigna el primer y único asiento a una persona
    DebeSerCierto((reserva_asiento(20) == 1));
    ESTADO("Al reservar el asiento");
    /// Una vez está lleno no permite más reservas
    DebeSerCierto((reserva_asiento(20) == -1));
    DebeSerCierto((reserva_asiento(-20) == -1));
    DebeSerCierto((reserva_asiento(0) == -1));

    /// Podemos liberar y volver a reservar
    DebeSerCierto((libera_asiento(1) == 20));
    ESTADO("Al liberar");

    DebeSerCierto((reserva_asiento(30) == 1));
    ESTADO("Al liberar se puede volver a reservar");

    /// No se permite liberar asientos ya libres o no existentes
    int ids[] = {2, -1, 0, 10, 1, 1};
    int results[] = {-1, -1, -1, -1, 30, -1};
    CHECK_LIBERA(6, ids, results);

    elimina_sala();
    ESTADO("Al eliminar la sala");
    FIN_TEST("Sala con 1 asiento");
}

void test_2asientos(){
    INICIO_TEST_NOMBRE("Sala con 2 asientos");
    DebeSerCierto(crea_sala(2) ==2);
    DebeSerCierto(capacidad_sala()==2);
    DebeSerCierto(asientos_libres()==2);
    DebeSerCierto(asientos_ocupados() == 0);

    ESTADO("Al crear una sala");

    /// Reservamos los 2 asientos
    DebeSerCierto(reserva_asiento(20) == 1);
    DebeSerCierto(asientos_ocupados()==1);
    DebeSerCierto(capacidad_sala()==2);
    DebeSerCierto(asientos_libres()==1);
    DebeSerCierto(reserva_asiento(21)==2);
    DebeSerCierto(asientos_ocupados()==2);

    ESTADO("Al reservar 2 asientos");

    /// Cuando la sala está llena no admite más reservas

    DebeSerCierto(reserva_asiento(-20)==-1);
    DebeSerCierto(reserva_asiento(20)==-1);
    DebeSerCierto(reserva_asiento(0)==-1);
    DebeSerCierto(asientos_libres()==0);
    DebeSerCierto(asientos_ocupados()==2);
    DebeSerCierto(capacidad_sala()==2);

    ESTADO("No permite reservar con ID inválido ni cuando la sala está llena");

    /// Podemos liberar y volver a reservar
    DebeSerCierto(libera_asiento(1)==20);
    DebeSerCierto(asientos_libres()== asientos_ocupados()==1);
    DebeSerCierto(capacidad_sala()==2);

    ESTADO("Al liberar");

    DebeSerCierto(reserva_asiento(20)==1);
    DebeSerCierto(capacidad_sala()== 2);
    DebeSerCierto(asientos_ocupados()==2);

    ESTADO("Y al volver a reservar");

    /// No podemos liberar cuando ya está libre o con id inválido
    DebeSerCierto(libera_asiento(2)==21);
    DebeSerCierto(asientos_libres()== asientos_ocupados()==1);
    DebeSerCierto(capacidad_sala()==2);
    DebeSerCierto(reserva_asiento(30)==2);
    DebeSerCierto(capacidad_sala()==2);
    DebeSerCierto(asientos_ocupados()==2);
    int ids[] = {-1, 0, 1, 1, 2, 2, 3,};
    int results[] = {-1, -1, 20, -1, 30, -1, -1};
    CHECK_LIBERA(6, ids, results);

    elimina_sala();

    ESTADO("Al eliminar la sala");
    FIN_TEST("Sala con 2 asientos");

}

void test_completo1() {
    INICIO_TEST_NOMBRE("Test completo 1");
    DebeSerCierto(crea_sala(250)==250);
    DebeSerCierto(capacidad_sala()==250);
    DebeSerCierto(asientos_ocupados() == 0);
    ESTADO("Al crear una sala");

    for (int i = 1; i <= 125; i++) {
        DebeSerCierto(reserva_asiento(i)==i);
    }

    ESTADO("Al reservar 125 asientos");

    DebeSerCierto(reserva_asiento(-20)==-1);

    ESTADO("No se reservan asientos con ID inválido");

    DebeSerCierto(asientos_ocupados() == asientos_libres());

    for (int i = 30; i < 80; i++) {
        DebeSerCierto(estado_asiento(i) > 0);
        DebeSerCierto(libera_asiento(i) == i);
        DebeSerCierto(estado_asiento(i) == 0);
    }
    DebeSerCierto(asientos_ocupados() + asientos_libres() == capacidad_sala());
    DebeSerCierto(asientos_ocupados() == 75);
    DebeSerCierto(asientos_libres() == 175);

    ESTADO("Al liberar los asientos 30 a 80");

    for(int i = 1; i <= capacidad_sala(); i+=2){
        DebeSerCierto(estado_asiento(reserva_asiento(i)) == i);
    }

    ESTADO("Al reservar con ids pares");

    elimina_sala();

    DebeSerCierto(capacidad_sala() == -1);
    DebeSerCierto(asientos_ocupados() == -1);
    DebeSerCierto(asientos_libres() == -1);
    DebeSerCierto(estado_asiento(1) == -1);
    DebeSerCierto(reserva_asiento(2) == -1);
    DebeSerCierto(libera_asiento(3) == -1);

    ESTADO("Al eliminar la sala");
    FIN_TEST("Completo 1");
}

void test_completo2() {
    INICIO_TEST_NOMBRE("Test completo 2");

    /// Arrays de prueba
    int personas_correctas [] = {1, 2, 3, 4};
    int personas_repedidas [] = {1, 1, 2, 2};
    int personas_ceros [] = {1, 0, 2};
    int personas_negativas[] = {1, -1, 2};

    /// No se debe poder realizar una reserva sin una sala creada

    if (DETALLES) puts("\nNo podemos reservar asientos si no hay ninguna sala creada:");

    DebeSerCierto(reserva_multiple(4, personas_correctas) == -1);
    DebeSerCierto(crea_sala(-1) == -1);
    DebeSerCierto(crea_sala(0) == -1);

    if(DETALLES) puts("\nCreamos una sala con 3 asientos e intentamos reservar 4 asientos (no hay plazas para todos):");

    crea_sala(3);

    DebeSerCierto(reserva_multiple(4, personas_correctas) == -1);
    DebeSerCierto(reserva_multiple(4, personas_repedidas) == -1);
    DebeSerCierto(reserva_multiple(4, personas_ceros) == -1);
    DebeSerCierto(reserva_multiple(4, personas_negativas) == -1);

    ESTADO("Estado de la sala");

    if (DETALLES) puts("Se pueden reservar múltiples asientos para la misma persona: \n- Reserva múltiple 1: {1}\n"
                            "- Reserva múltiple 2: {1, 1}\n(Los intentos fallidos corresponden con reservas de 0 personas.)\n");


    DebeSerCierto(reserva_multiple(1, personas_correctas) == 1);
    DebeSerCierto(reserva_multiple(0, personas_repedidas) == -1);
    DebeSerCierto(reserva_multiple(2, personas_repedidas) == 2);
    DebeSerCierto(reserva_multiple(0, personas_repedidas) == -1);

    ESTADO("Estado de la sala");
    if (DETALLES) puts("Si no hay asientos libres no se puede reservar nada:");

    DebeSerCierto(reserva_multiple(1, personas_repedidas) == -1);

    ESTADO("Estado de la sala");
    if (DETALLES) printf("No se puede reservar más de los asientos libres (reserva múltiple con 2 personas, sala con 1 asiento libre).\n");

    DebeSerCierto(libera_asiento(4) == -1);
    DebeSerCierto(libera_asiento(3) == 1);
    DebeSerCierto(asientos_libres() == 1);


    ESTADO("Al liberar el asiento");
    if (DETALLES) puts("Al intentar reservar más asientos de los disponibles:\n");

    DebeSerCierto(reserva_multiple(2, personas_correctas));

    ESTADO("Estado de la sala");
    if (DETALLES) puts("Liberamos todos los asientos ocupados e intentamos realizar reservas inválidas:\n");

    DebeSerCierto(libera_asiento(2) == 1);
    DebeSerCierto(levantarse(1) == 1);
    DebeSerCierto(asientos_libres() == 3);
    DebeSerCierto(reserva_multiple(3, personas_negativas));
    DebeSerCierto(reserva_multiple(3, personas_ceros));

    ESTADO("Estado de la sala");
    if(DETALLES) puts("Creamos una sala de 20 asientos:\n");

    DebeSerCierto(crea_sala(20) == 20);
    DebeSerCierto(capacidad_sala()  == 20);
    DebeSerCierto(asientos_libres() == 20);
    DebeSerCierto(asientos_ocupados() == 0);

    ESTADO("Estado de la sala");
    if (DETALLES) puts("Reservamos 10 asientos con reserva múltiple:\n");

    DebeSerCierto(reserva_multiple(4, personas_correctas) == 4);
    DebeSerCierto(reserva_multiple(4, personas_correctas) == 4);
    DebeSerCierto(reserva_multiple(1, personas_negativas) == 1);
    DebeSerCierto(reserva_multiple(1, personas_ceros) == 1);

    ESTADO("Estado de la sala");
    if (DETALLES) puts("Reservamos el resto de asientos con sentarse:\n");

    int ids_personas = 40;
    while(sentarse(ids_personas++) != -1);
    DebeSerCierto(capacidad_sala() == asientos_ocupados());
    DebeSerCierto(asientos_libres() == 0);

    ESTADO("\n** Los asientos con id > 40 se han reservado con la función sentarse **");

    int id_asiento = 2;
    while (libera_asiento(id_asiento) != -1){id_asiento += 2;}

    ESTADO("Después de liberar los asientos pares");
    if (DETALLES) puts("Llenamos los asientos pares liberados con la función sentarse:\n");


    while(sentarse(ids_personas++) != -1);
    DebeSerCierto(capacidad_sala() == asientos_ocupados());
    DebeSerCierto(asientos_libres() == 0);

    ESTADO("Estado de la sala");

    /// Liberamos asientos no contiguos
    int asientos[] = {1, 3, 6, 9, 12, 16, 20};

    for (int i = 0; i < 7; i++) libera_asiento(asientos[i]);

    ESTADO("Después de liberar 7 asientos no contiguos");
    if (DETALLES) puts("Después de reservar todos los asientos con reservas múltiples y contemplar casos erróneos:\n");

    /// Casos erróneos
    if (DETALLES) puts("Casos erróneos (ids/número personas incorrectos):");
    DebeSerCierto(reserva_multiple(4, personas_negativas) == -1);
    DebeSerCierto(reserva_multiple(4, personas_ceros) == -1);
    DebeSerCierto(reserva_multiple(-1, personas_correctas) == -1);
    DebeSerCierto(reserva_multiple(0, personas_correctas) == -1);

    /// Reserva correcta de 4 asientos
    if (DETALLES) puts("\nReserva correcta de 4 asientos:");
    DebeSerCierto(reserva_multiple(4, personas_correctas) == 4);

    /// Más casos erróneos
    if(DETALLES) puts("\nMás casos erróneos (ids inválidos/falta de espacio):");
    DebeSerCierto(reserva_multiple(4, personas_correctas) == -1);
    DebeSerCierto(reserva_multiple(3, personas_ceros) == -1);
    DebeSerCierto(reserva_multiple(3, personas_negativas) == -1);

    /// Reservamos los asientos restantes
    if (DETALLES) puts("\nReserva correcta de 3 asientos restantes:");
    DebeSerCierto(reserva_multiple(3, personas_repedidas) == 3);

    ESTADO("Estado de la sala");

    DebeSerCierto(capacidad_sala() == asientos_ocupados());
    DebeSerCierto(asientos_libres() == 0);
    if (DETALLES) puts("Si la sala está llena no se pueden reservar más asientos con ninguna de las funciones:\n");
    DebeSerCierto(reserva_asiento(1) == -1);
    DebeSerCierto(sentarse(1) == -1);
    DebeSerCierto(reserva_multiple(2, personas_correctas) == -1);
    DebeSerCierto(reserva_multiple(1, personas_correctas) == -1);
    DebeSerCierto(reserva_multiple(0, personas_correctas) == -1);

    elimina_sala();

    ESTADO("-------------------\nAl eliminar la sala");
    FIN_TEST("Completo 2");

}

void test_reserva_multiple() {

    INICIO_TEST_NOMBRE("Test reserva múltiple");

    DebeSerCierto(crea_sala(30)==30);
    DebeSerCierto(crea_sala(30)==30);

    ESTADO("Creamos una sala");

    int lista[] = {21, 11, 2, 4};

    if (DETALLES) puts("Reserva múltiple de 2 asientos:\n");
    reserva_multiple(2, lista);
    ESTADO("Estado de la sala");

    DebeSerCierto(asientos_ocupados() == 2);

    if (DETALLES) puts("Reserva múltiple de 3 asientos:\n");
    reserva_multiple(3, lista);
    ESTADO("Estado de la sala");

    DebeSerCierto(crea_sala(20) == 20);

    ESTADO("Creamos una nueva sala (sobreescribe los datos de la anterior)");

    DebeSerCierto(asientos_ocupados() == 0);

    if (DETALLES) puts("Reserva múltiple de 4 asientos:\n");
    reserva_multiple(4, lista);
    ESTADO("Estado de la sala");

    DebeSerCierto(asientos_ocupados() == 4);

    if (DETALLES) puts("Reserva de 1 asiento1:\n");
    reserva_multiple(1, lista);
    ESTADO("Estado de la sala");

    DebeSerCierto(asientos_ocupados() == 5);

    if (DETALLES) puts("Reserva de 0 asientos:\n");
    reserva_multiple(0, lista);
    ESTADO("Estado de la sala");

    DebeSerCierto(asientos_ocupados() == 5);


    int listaGrande[] = {4, 3, 8, 10, 24, 12, 34, 1, 9, 12, 1, 43,
                         12, 1, 34, 21, 20, 20, 70};

    if (DETALLES) puts("No se permite reservar a ninguna persona si se pretenden reservar más asientos de los que están libres:\n");
    DebeSerCierto(reserva_multiple(19, listaGrande) == -1);
    ESTADO("Estado de la sala");

    int listaInvalida1[] = {1, 2, 3, 4, -1};
    int listaInvalida2[] = {1, 2, 3, 4, 0};

    if (DETALLES) puts("No se permite reservar a ninguna persona si hay alguna con un id inválido:\n");
    DebeSerCierto(reserva_multiple(5, listaInvalida1) == -1);
    DebeSerCierto(reserva_multiple(5, listaInvalida2) == -1);
    ESTADO("Estado de la sala");

    elimina_sala();
    DebeSerCierto(asientos_ocupados() == -1);

    ESTADO("Al eliminar la sala");
    FIN_TEST("Reserva múltiple");
}

void ejecuta_tests ()
{
    test_reserva_multiple();
    test_capacidad_incorrecta();
    test_ReservaBasica();
    test_1asiento();
    test_2asientos();
    test_completo1();
    test_completo2();
    test_completo_practica_3();
}

void test_completo_practica_3() {
    INICIO_TEST_NOMBRE("Completo 3");
    DebeSerCierto(crea_sala(10) == 10);
    DebeSerCierto(gruada_estado_sala()

void main()
{
    puts("Iniciando tests...");

    ejecuta_tests();

    puts("Batería de test completa.");
}
