/// TESTS

#define DebeSerCierto(x)	assert(x)
#define DebeSerFalso(x)		assert(!(x))

#define INICIO_TEST_NOMBRE(nombre)              if (DETALLES) INICIO_TEST(nombre)
#define ESTADO(titulo)                          if (DETALLES) estado_sala(titulo)
#define CHECK_LIBERA(nCheck, ids, results)\
                for (int i = 0; i < nCheck; i++)\
                DebeSerCierto(libera_asiento(ids[i]) == results[i]);


/// Control ficheros

#define CHECK_LEIDO(x)\
    if ((x) == -1) {\
        perror("Error en la lectura");\
		close(x);\
        return -1;\
    }

#define CHECK_ERROR(x)\
	   if ((x) == -1) {\
	   perror("Error en la apertura");\
	   return -1;\
	}

#define CHECK_ESCRITO(x)\
	   if ((x) == -1) {\
	        perror("Error en la escritura del archivo");\
		close(x);\
		return -1;\
	    }

#define SELECT_DATOS_SALA(fd, control)\
	int datos_sala [2];\
	ssize_t bytes_leidos = read(fd, &datos_sala, sizeof(int)*2);\
	CHECK_LEIDO(bytes_leidos);\
	\
	if(control && capacidad_sala() != datos_sala[0]){\
                perror("La sala creada tiene una capacidad distinta a la que quiere restaurar");\
                close(fd);\
		return -1;\
	}
	

/// Control misala

#define ASIENTO_CORRECTO(x)\
	if (x >= capacidad || x <= 0) {\
		*(asientos_invalidos + n_asientos_invalidos) = x;\
		 n_asientos_invalidos++;\
	} else {\
		*(asientos + n_asientos) = x;\
		n_asientos++;\
	}
			
#define PERSONA_CORRECTA(x)\
 	int asiento_ocupado = levantarse(x);\
 	if (asiento_ocupado == -1) {\
 		*(asientos_invalidos + n_asientos_invalidos) = x;\
 		n_asientos_invalidos++;\
 	} else {\
 		*(asientos + n_asientos) = asiento_ocupado;\
 		n_asientos++;\
 	}
 	
#define CHECK_NARGUMENTOS(x, y)\
		if (!(argc >= x && y) || !(argc == x && y)) {\
			fprintf(stderr, "Error en comando: Número de argumentos incorrecto.\n");\
			exit(1);\
		}

#define CREA_SALA(x, fd)\
		if (crea_sala(x) == -1) {\
			if (fd > 0) close(fd);\
			fprintf(stderr, "Error en la creación de la sala.\n");\
			exit(1);\
		}
		
#define GUARDA(sala, fd)\
		if (guarda_estado_sala(dir) == -1) {\
			if (sala) elimina_sala();\
			if (fd > 0) close(fd);\
			perror("No se pudo guardar el estado de la sala");\
			exit(1);\
		}
		
#define RECUPERA(sala, fd)\
                if (recupera_estado_sala(dir) == -1) {\
			if (sala) elimina_sala();\
			if (fd > 0) close(fd);\
			perror("No se pudo recuperar el estado de la sala");\
			exit(1);\
		}
