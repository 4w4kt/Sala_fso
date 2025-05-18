// Ejemplo sencillo de 6 hilos que se ejecutan de forma concurrente.
// Compilar con la biblioteca pthreads. Ejemplo:
// 	gcc prueba_hilos_v4.c -o hilos -lpthread
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

//Número de hilos que se van a lanzar
#define MAX_HILOS 6

void retardo_aleatorio()
{
  unsigned long retardo=rand()%1000000;
  usleep(retardo);
}

// Objetos de tipo hilo
pthread_t hilo[MAX_HILOS];

// Las funciones que ejecutan los hilos SIEMPRE deben respetar ese formato:
// void* nombre_de_funcion (void*);

// Rutina que ejecutarán los hilos
void* rutina(void* arg)
{
  int i,param;
 
  param=(int)arg;
  for (i=1; i<=10;i++)
  {
    retardo_aleatorio();
    printf("*** soy el HILO %d\n",param);
    fflush(stdout);
  }
  printf("*** HILO %d - the end\n",param);
  fflush(stdout);
  return NULL;
}

// Aquí empieza el programa
int main()
{
  int i,ret;

  // Lanzo 3 hilos que ejecuta: rutina(i+1)
  for (i=0; i<=2;i++)
  {
    ret=pthread_create(&hilo[i], NULL, rutina, (void*)(i+1)); 
    if (ret) {
	errno=ret;
	fprintf(stderr,"Error %d pthread_create al crear el hilo %d: %s\n",errno,i,strerror(errno));
	exit(-1);
    }	 
  }
 
  // Espero a que terminen los 3 hilos lanzados
  // La variable dummy no la usamos en este ejemplo.
  void* dummy;
 
  for (i=0; i<=2;i++)
  {
    ret=pthread_join(hilo[i],&dummy);
    if (ret) {
	errno=ret;
	fprintf(stderr,"Error %d pthread_create al crear el hilo %d: %s\n",errno,i,strerror(errno));
	exit(-1);
    }
  } 

 // Lanzo otros 3 hilos que ejecuta: rutina(i+1)
  for (i=3; i<=5;i++)
  {
    ret=pthread_create(&hilo[i], NULL, rutina, (void*)(i+1)); 
    if (ret) {
	errno=ret;
	fprintf(stderr,"Error %d pthread_create al crear el hilo %d: %s\n",errno,i,strerror(errno));
	exit(-1);
    }	 
  }
 
  // Espero a que terminen los 3 hilos lanzados
  // La variable dummy no la usamos en este ejemplo.
 
  for (i=3; i<=5;i++)
  {
    ret=pthread_join(hilo[i],&dummy);
    if (ret) {
	errno=ret;
	fprintf(stderr,"Error %d pthread_create al crear el hilo %d: %s\n",errno,i,strerror(errno));
	exit(-1);
    }
  } 
  
  pthread_exit(NULL);
}








