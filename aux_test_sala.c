//
// Created by aleja on 11/03/2025.
//

#include <stdio.h>
#include "sala.h"

/**
 * Muestra el estado de todos y cada uno de los asientos
 * y una información resumen de la sala.
 */
int estado_sala(char* titulo) {
    if (capacidad_sala() == -1) {
        puts("La sala no existe.\n-------------------\n");
        return -1;
    }
    
    printf("%s\n", titulo);

    printf("Estado de los asientos: {");
    for (int i = 1; i<capacidad_sala(); i++){
        printf("%d, ", estado_asiento(i));
    }
    printf("%d} \n", estado_asiento(capacidad_sala()));
    printf("- Asientos libres = %d \n- Asientos ocupados = %d \n- Capacidad de la sala = %d \n-------------------\n",
           asientos_libres(), asientos_ocupados(), capacidad_sala());
    return 0;
}

/**
 * Libera un asiento ocupado por la persona pasado por parametro
 * @param id_persona identificador de la persona que va a liberar el asiento
 * @return número del asiento liberado, -1 en caso contrario
 */
int levantarse(int id_persona) {
    if (id_persona <= 0 || asientos_ocupados() == capacidad_sala()) {
        if (DETALLES) printf("Ha ocurrido un error.\n");
        return -1;
    }
    for (int i = 1; i <= capacidad_sala(); i++) {
        if (estado_asiento(i) == id_persona) {
            int result = libera_asiento(i);

            if (DETALLES) printf("%d se ha levantado del asiento %d\n", id_persona, i);
            return result;
        }
    }
    if (DETALLES) printf("No se ha encontrado %d en esta sala\n", id_persona);
    return -1;
}

/**
 * encuentra un asiento para la persona pasada por parámetro
 * @param id_persona identificador de la persona que va a ocupar el asiento
 * @return el número de asiento donde se puede sentar, -1 en si no es posible asignarle un asiento
 */
int sentarse(int id_persona) {

    int result = reserva_asiento(id_persona);
    if (DETALLES) {
        if (result != -1) {

            printf("Puede sentarse en el asiento %d\n", result);
            return result;
        }
        printf("No se ha encontrado un asiento para usted, %d\n", id_persona);
        return result;
    }
    return result;

}

/**
 * Reserva un asiento a cada persona pasada
 * @param n_personas cantidad de personas en la lista
 * @param lista_id lista de personas que desean un asiento
 * @return el número de personas si se a encontrado asiento para todas, -1 en caso contrario
 */

int reserva_multiple(int n_personas, int* lista_id) {
    if (capacidad_sala() < n_personas + asientos_ocupados() || n_personas <= 0){
        if (DETALLES){
            puts("No se pudo realizar la reserva.");
        }
        return -1;
    }
    for (int i = 0; i < n_personas; i++){
        if (*(lista_id+i) <=0){
            if (DETALLES){
                puts("No se pudo realizar la reserva.");
            }
            return -1;
        }
    }

    int asientos[n_personas];
    for (int i = 0; i < n_personas; i++){

        asientos[i] = reserva_asiento(*(lista_id + i));
    }
    if (DETALLES){
        printf("Los asientos reservados fueron {");
        for (int i = 0; i<n_personas -1; i++){
            printf("%d, ", asientos[i]);
        }
        printf("%d} \n", asientos[n_personas-1]);
    }


    return n_personas;
}
