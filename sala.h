//
// Created by aleja on 01/03/2025.
//
#include <stddef.h>

# ifndef PROCESOSPRAC2_ARS_JMGF_SALA_H
# define PROCESOSPRAC2_ARS_JMGF_SALA_H

# ifndef DETALLES
# define DETALLES 1
# endif

#define CHECK_LEIDO(x)\
    if ((x) == -1) {\
        perror("Error en la lectura");\
		close(x);\
        return -1;\
    }

#define CHECK_ERROR(x)\
	   if ((x) == -1) {            \
	   perror("Error en la apertura"); \
	   return -1;              \
	}

#define CHECK_ESCRITO(x)         \
	   if ((x) == -1) {            \
	   perror("Error en la escritura del archivo");		\
		close(x);			\
		return -1;			\
	}

#define SELECT_DATOS_SALA(fd, control) \
	int datos_sala [2];								\
	ssize_t bytes_leidos = read(fd, &datos_sala, sizeof(int)*2);			\
	CHECK_LEIDO(bytes_leidos);                    									\
	if(control && capacidad_sala() != datos_sala[0]){							\
		perror("La sala creada tiene una capacidad distinta a la que quiere restaurar");		\
        close(fd);                                              \
		return -1;				\
	}
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

extern int reserva_multiple(int	n_personas,	int* lista_id);

/// Clase practica3

extern int guarda_estado_sala(char* ruta_fichero);

extern int recupera_estado_sala(char* ruta_fichero);

extern int guarda_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos);

extern int recupera_estado_parcial_sala (char* ruta_fichero, size_t num_asientos, int* id_asientos);


# endif
