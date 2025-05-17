// Ejemplo sencillo de dos hilos que se ejecutan de forma concurrente.
// Compilar con la biblioteca pthreads. Ejemplo:
// 	gcc prueba_hilos_v5.c -o hilos -lpthread
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

void retardo_aleatorio()
{
  unsigned long retardo=rand()%1000000;
  usleep(retardo);
}

// Objetos de tipo hilo
pthread_t un_hilo;
pthread_t otro_hilo;

// Las funciones que ejecutan los hilos SIEMPRE deben respetar ese formato:
// void* nombre_de_funcion (void*);

// Rutina que ejecutará uno de los hilos
void* rutina1(void* arg)
{
  int i;
  int *ret;

  for (i=1; i<=10;i++)
  {
    retardo_aleatorio();
    printf("*** soy el HILO 1\n");
    fflush(stdout);
  }
  puts("*** HILO1 - the end\n");
  
// Retornamos información de estado al terminar
// Conveniente hacerlo empleando zonas de memoria que dejen de existir al terminar el hilo
// Por ejemplo, sería un error hacerlo a través de una variable local del hilo,
// pues estas variables se guardan en la pila, zona de memoria que es liberada tras la terminación del hilo
// En este ejemplo, vamos a hacerlo a través de un reserva de memoria que hacemos con malloc y vamos a devolver un valor entero

  ret=(int*)malloc(sizeof(int));
  *ret=5;
  pthread_exit((void*)ret);
}

// Rutina que ejecutará el otro hilo
void* rutina2(void* arg)
{
  int i;
  int *ret;

  for (i=1; i<=10;i++)
  {
    retardo_aleatorio();
    printf("@@@ soy el HILO 2\n");
    fflush(stdout);
  }
  puts("@@@ HILO2 - the end\n");
  ret=(int*)malloc(sizeof(int));
  *ret=7;
  pthread_exit((void *)ret);
}

// Aquí empieza el programa
int main()
{
  int ret;

  // Lanzo un hilo que ejecuta: rutina1(NULL)
  ret=pthread_create(&un_hilo, NULL, rutina1, NULL); 
  if (ret) {
	errno=ret;
	fprintf(stderr,"Error %d pthread_create al crear el hilo1: %s\n",errno,strerror(errno));
	exit(-1);
  } 
 
  // Lanzo otro hilo que ejecuta: rutina2(NULL)
  ret=pthread_create(&otro_hilo, NULL, rutina2, NULL); 
  if (ret) {
	errno=ret;
	fprintf(stderr,"Error %d pthread_create al crear el hilo2: %s\n",errno,strerror(errno));
	exit(-1);
  }
 
  // El main() espera a que acaben los dos hilos.
  // Si no hacemos pthread_join(), el programa finaliza sin que 
  // los hilos hayan acabado.
  //Recuperamos el estado de terminación del hilo 1 a través del puntero dummy
  void* dummy;
  
  ret=pthread_join(un_hilo,&dummy);
  if (ret) {
	errno=ret;
	fprintf(stderr,"Error %d pthread_join sobre hilo1: %s\n",errno,strerror(errno));
	exit(-1);
  }

 //Mostramos el estado de terminación del hilo 1
  printf("**** Hilo 1: estado terminación %d\n",*(int*)dummy);

  //Recuperamos el estado de terminación del hilo 2 a través del puntero dummy
  ret=pthread_join(otro_hilo,&dummy);
  if (ret) {
	errno=ret;
	fprintf(stderr,"Error %d pthread_join sobre hilo2: %s\n",errno,strerror(errno));
	exit(-1);
  }

  //Mostramos el estado de terminación del hilo 2
  printf("*** Hilo 2: estado terminación %d\n",*(int*)dummy);
  pthread_exit(NULL);
}








