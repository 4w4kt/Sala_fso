// Utilidad para generar pausas aleatorias
// Se pueden utilizar en las prácticas de programación concurrente

// descomentar esta línea para compilar en modo depuración
// (contiene un test de la función pausa_aleatoria)
// #define DEBUG

#include <assert.h>
#include <time.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#include <sys/time.h>
#endif

// Genera una pausa aleatoria entre 0 y max_segundos
void pausa_aleatoria (float max_segundos);

static struct timespec calcula_pausa_aleatoria (float max_segundos) {
  const long MILLARDO = 1000000000L;
  assert (RAND_MAX >= MILLARDO);

  struct timespec pausa;
  if (max_segundos < 1.0) 
  {
    pausa.tv_sec = 0;
    long tope = (long)(max_segundos*MILLARDO);
    pausa.tv_nsec = rand() % tope;
  } 
  else 
  {
    long segundos = (long)max_segundos;
    long nanosegundos = (long)((max_segundos-segundos)*MILLARDO);
    do {
      pausa.tv_sec = rand() % (segundos+1);
      pausa.tv_nsec = rand() % MILLARDO;      
    } while (pausa.tv_sec==segundos && pausa.tv_nsec>nanosegundos);
#ifdef DEBUG
    printf("(sec=%ld nsec=%ld)",pausa.tv_sec,pausa.tv_nsec); 
    fflush(stdout);
#endif
  }
  return pausa;
}

static void ejecuta_pausa(struct timespec* pausa) {
  struct timespec restante = { .tv_sec=0, .tv_nsec=0 };
  for(;;) {
    nanosleep(pausa,&restante);
    if (restante.tv_sec>0 || restante.tv_nsec>0) {
      *pausa = restante;
    }
    else break;
  }  
}

// Genera una pausa aleatoria entre 0 y max_segundos
void pausa_aleatoria (float max_segundos) {
  assert (max_segundos>0);
  struct timespec pausa = calcula_pausa_aleatoria(max_segundos);
  ejecuta_pausa(&pausa);
}


// TESTS DEL MÓDULO

#ifdef DEBUG
static void test_lote_pausas (int npausas, float promedio) {
  printf("inicio %d pausas aleatorias de hasta %f segundos...",
         npausas,promedio);
  fflush(stdout);
  for (int i=1;i<=npausas;i++) {
    printf("%d ",i);
    fflush(stdout);
    pausa_aleatoria(promedio);
  }
  puts("hecho");
}


static void test_pausas_aleatorias()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  srand(tv.tv_usec);
  test_lote_pausas(10,0.5);
  test_lote_pausas(20,0.1);
  test_lote_pausas(10,2.5);
}

int main() { test_pausas_aleatorias(); }
#endif
