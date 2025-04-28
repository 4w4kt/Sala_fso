
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include "sala.h"



int crea_test_sh(char* metodo){
    pid_t pid_creado = fork();
    if (pid_creado == -1) {
        perror("Se produjo un error al crear la sucursal.");
        exit(1);
    }
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

    int setup(){
        int fd = open(sala_vacia.txt, O_RDWR|O_CREAT|O_TRUNC);
        CHECK_ERROR (fd);
        close(fd);
        fd = open(sala_creada.txt, O_RDWR|O_CREAT|O_TRUNC);
        CHECK_ERROR (fd);
        crea_sala(20);
        int* ids [20];
        for(int i = 1; i <= 20; i++){
            ids[i] = i*3;
        }
        reserva_multiple(20, ids);
        levantarse_asiento(3);
        levantarse_asiento(9);
        guarda_estado_sala(sala_creada.txt);
        close(fd);
        return 0;

        fd = open("sin_permisos.txt", O_CREAT | O_WRONLY | O_TRUNC, 0000);
        if (fd == -1) {
            perror("Error creando el archivo");
            return 1;
        }
        close(fd);    
    }

    int test_crea() {
        // argumentos incompletos
        assert(crea_test_sh("./misala crea -c 20") == 1);
        assert(crea_test_sh("./misala crea -f sala_f_1.txt") == 1);
        assert(crea_test_sh("./misala crea -fo sala_fo_1.txt") == 1);ç

        // no existe
        assert(crea_test_sh("./misala crea -f sala_f_1.txt -c 20") == 0);
        assert(crea_test_sh("./misala crea -fo sala_fo_1.txt -c 20") == 0);

        // existe
        assert(crea_test_sh("./misala crea -f sala_f_1.txt -c 20") == 1);
        assert(crea_test_sh("./misala crea -fo sala_fo_1.txt -c 20") == 0);

        // capacidad negativa && existe
        assert(crea_test_sh("./misala crea -f sala_f_1.txt -c -20") == 1);
        assert(crea_test_sh("./misala crea -fo sala_fo_1.txt -c -20") == 1);
        // capacidad negativa && no existe 
        assert(crea_test_sh("./misala crea -f noexiste.txt -c -20") == 1);
        assert(crea_test_sh("./misala crea -fo noexiste.txt -c -20") == 1);

        // no tengo permisos
        assert(crea_test_sh("./misala crea -f sin_permisos.txt -c 20") == 1);
        assert(crea_test_sh("./misala crea -fo sin_permisos.txt -c 20") == 1);
    }
    
    int test_reserva(){
        // no existe
        assert(crea_test_sh("./misala reserva -f noexiste.txt 1 2 3 4") == 1);
        // no tengo permisos
        assert(crea_test_sh("./misala reserva -f sin_permisos.txt 1 2 3 4") == 1);
        // existe
        assert(crea_test_sh("./misala reserva -f sala_creada.txt 1 2 3 4") == 0);
        // faltan un argumentos
        assert(crea_test_sh("./misala reserva -f sala_creada.txt") == 1);

    }










int main(){
    setup();
    test_crea();
    test_reserva();

}
