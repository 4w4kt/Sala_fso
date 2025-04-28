
#include "sala.h"
#include <stdlib.h>

#define PREPARAHIJO \
    pid_t pid_creado = fork(); \
    if (pid_creado == -1) {                             \
        perror("Se produjo un error al crear la sucursal."); \
        exit(1);                            \
    }\

int crea_test_sh(char* metodo){
    PREPARAHIJO
    if (pid_creado == 0) {

        int ret = execlp("sh", "sh", "-c", metodo, NULL);
        if (ret == -1) {
            perror("No se pudo crear una nueva sala.");
            exit(1);
        }

    }
    int status;
    pid_t pidhijo = wait(&status);
    return WEXITSTATUS(status);
    }

int crea_test(char* parametros){
    PREPARAHIJO
    if (pid_creado == 0) {

    int ret = execvp(parametros[0], parametros);
        if (ret == -1) {
            perror("No se pudo crear una nueva sala.");
            exit(1);
        }
    }
    int status;
    pid_t pidhijo = wait(&status);
    return WEXITSTATUS(status);

}

int test_crea(){

}












int main(){
    test_crea();
}




/*
#include <stdio.h>    // Para perror, printf
#include <stdlib.h>   // Para exit, malloc
#include <unistd.h>   // Para fork, execvp, execlp
#include <sys/types.h> // Para pid_t
#include <sys/wait.h>  // Para wait, WEXITSTATUS
#include "sala.h"     // Tu cabecera personalizada (está bien dejarla si la necesitas)

// Macro para crear el hijo
#define PREPARAHIJO \
    pid_t pid_creado = fork(); \
    if (pid_creado == -1) { \
        perror("Error creando el hijo"); \
        exit(EXIT_FAILURE); \
    }

// Lanza usando "sh -c"
int crea_test_sh(const char *comando){
    PREPARAHIJO
    if (pid_creado == 0) {
        int ret = execlp("sh", "sh", "-c", comando, NULL);
        if (ret == -1) {
            perror("Error en execlp");
            exit(EXIT_FAILURE);
        }
    }
    int status;
    waitpid(pid_creado, &status, 0);
    return WEXITSTATUS(status);
}

// Lanza directamente con execvp
int crea_test(char *const argv[]){
    PREPARAHIJO
    if (pid_creado == 0) {
        int ret = execvp(argv[0], argv);
        if (ret == -1) {
            perror("Error en execvp");
            exit(EXIT_FAILURE);
        }
    }
    int status;
    waitpid(pid_creado, &status, 0);
    return WEXITSTATUS(status);
}

// Test de ambos métodos
int test_crea() {
    printf("=== Test usando execlp (sh -c) ===\n");
    int resultado_sh = crea_test_sh("./mi_sala crea -f fichero1.txt");
    printf("Resultado crea_test_sh: %d\n", resultado_sh);

    printf("\n=== Test usando execvp ===\n");
    char *args[] = {"./mi_sala", "crea", "-f", "fichero1.txt", NULL};
    int resultado_execvp = crea_test(args);
    printf("Resultado crea_test: %d\n", resultado_execvp);

    return 0;
}



*/