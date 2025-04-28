
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


    int test_crea() {
        // no existe
        assert(crea_test_sh("./mi_sala crea -f crear_sala_f_1.txt -c 20") == crea_test_sh("./mi_sala crea -fo crear_sala_fo_1.txt -c 20"));

        // existe
        assert(crea_test_sh("./mi_sala crea -f crear_sala_f_1.txt -c 20") == 1);
        assert(crea_test_sh("./mi_sala crea -fo crear_sala_fo_1.txt -c 20") == 0);

        // capacidad negativa && existe
        assert(crea_test_sh("./mi_sala crea -f crear_sala_f_1.txt -c -20") == 1);
        assert(crea_test_sh("./mi_sala crea -fo crear_sala_fo_1.txt -c -20") == 1);
        // capacidad negativa && no existe 
        assert(crea_test_sh("./mi_sala crea -f noexiste.txt -c -20") == 1);
        assert(crea_test_sh("./mi_sala crea -fo noexiste.txt -c -20") == 1);

        // no tengo permisos
        assert(crea_test_sh("./mi_sala crea -f sin_permisos.txt -c 20") == 1);
        assert(crea_test_sh("./mi_sala crea -fo sin_permisos.txt -c 20") == 1);
        
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








int main(){
    setup();
    test_crea();

}
