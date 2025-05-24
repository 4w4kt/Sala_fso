// Ejemplo sencillo con dos llamadas secuenciales a dos procedimientos.
// 	gcc prueba_procedimientos.c -o ejecutable
//

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

// Rutina llamada desde el main en primer lugar
void* rutina1(void* arg)
{
  int i;
  for (i=1; i<=10;i++)
  {
    retardo_aleatorio();
    printf("*** soy el HILO 1");
    fflush(stdout);
  }
  puts("*** HILO1 - the end");
  return NULL;
}

// Rutina llamada desde el main en segundo lugar
void* rutina2(void* arg)
{
  int i;
  for (i=1; i<=10;i++)
  {
    retardo_aleatorio();
    printf("@@@ soy el HILO 2");
    fflush(stdout);
  }
  puts("@@@ HILO2 - the end");
  return NULL;
}

// Aquí empieza el programa
int main()
{

  rutina1(NULL);
  rutina2(NULL);

  exit(0);
}








