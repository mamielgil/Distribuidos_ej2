#include "claves.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// ESTE CLIENTE CREA Y MODIFICA CONTINUAMENTE UNA CLAVE
int main() {
    printf("[ESCRITOR] Iniciando test de concurrencia...\n");
    
    char *key = "clave_compartida";
    char value1[256];
    float V_value2[4] = {1.1, 2.2, 3.3, 4.4};
    struct Paquete paquete = {10, 20, 30};

    for (int i = 0; i < 10; i++) {
        sprintf(value1, "value1%d", i);
        
        // Alternamos entre set y modify
        int res;
        if (i % 2 == 0) {
            res = set_value(key, value1, 4, V_value2, paquete);
            if(res == 0){
                printf("[ESCRITOR] SET: %s\n", value1);

            }else if(res == -1){
                printf("[ERROR] La key aún existe\n");
            }
        } else {
            res = modify_value(key, value1, 4, V_value2, paquete);

            if(res == 0){
                printf("[ESCRITOR] MODIFY: %s\n", value1);

            }else if(res == -1){
                printf("[ERROR] La key no existe: %s no se pudo escribir el nuevo valor1: %s\n", key, value1);
            }
        }
        
        // Pausa de 10 milisegundos para permitir que otros hilos entren
        usleep(10000); 
    }
    printf("[ESCRITOR] ¡Terminado!\n");
    return 0;
}