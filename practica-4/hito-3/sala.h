//
// Created by aleja on 01/03/2025.
//
#include <stddef.h>

# ifndef PROCESOSPRAC2_ARS_JMGF_SALA_H
# define PROCESOSPRAC2_ARS_JMGF_SALA_H

# ifndef DETALLES
# define DETALLES 0
# endif


/// Clase sala.c

extern int reserva_asiento(int id_persona);

extern int libera_asiento(int id_asiento);

extern int estado_asiento(int id_asiento);

extern int asientos_libres();

extern int asientos_ocupados();

extern int capacidad_sala();

extern int crea_sala(int capacidad);

extern int elimina_sala();

extern int reemplaza_sala(int* asientos, int capacidad, int asientos_ocupados);

extern int* get_sala();

extern int set_asiento(int id_asiento, int id_persona);

/// Clase aux_test_sala.c

extern int estado_sala(char* titulo);

extern int sentarse(int id_persona);

extern int levantarse(int id_persona);

extern int reserva_multiple(int	n_personas, int* lista_id);

extern int libera_cualquiera();

/// Clase practica3

extern int guarda_estado_sala(char* ruta_fichero);

extern int recupera_estado_sala(char* ruta_fichero);

extern int guarda_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos);

extern int recupera_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos);

/// Manejo de hilos

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

# endif
