#include "claves.h"
#include <stdio.h>
#include <unistd.h>


// ESTE CLIENTE CREAR Y BORRA CONTINUAMENTE UNA CLAVE

int main() {
    printf("[CAOS] Iniciando test de concurrencia...\n");

    for (int i = 0; i < 10; i++) {
        
        
        int res = delete_key("clave_compartida");
        if(res == 0){
            printf("[OK] Clave compartida borrada!\n");

        }else if(res == -1){
            printf("[ERROR] No se pudo borrar la clave compartida!\n");

        }
    usleep(2000);       
    }
    printf("[CAOS] ¡Terminado!\n");
    return 0;
}