//
// Created by aleja on 01/03/2025.
//

# ifndef PROCESOSPRAC2_ARS_JMGF_SALA_H
# define PROCESOSPRAC2_ARS_JMGF_SALA_H

# ifndef DETALLES
# define DETALLES 1
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


/// Clase aux_test_sala.c

extern int estado_sala(char* titulo);

extern int sentarse(int id_persona);

extern int levantarse(int id_persona);

extern int reserva_multiple(int	n_personas,	int* lista_id);

# endif
