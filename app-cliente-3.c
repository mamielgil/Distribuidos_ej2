#include "claves.h"
#include <stdio.h>
#include <unistd.h>

// ESTE CLIENTE OBTIENE CONTINUAMENTE UNA CLAVE CONCRETA

int main() {

    printf("[LECTOR] Iniciando test de concurrencia...\n");
    
    char *key = "clave_compartida";
    char value1[256];
    int N_value2;
    float V_value2[32];
    struct Paquete paquete;

    for (int i = 0; i < 10; i++) {
        
            int res = get_value(key, value1, &N_value2, V_value2, &paquete);
            if (res == 0) {
                printf("[LECTOR] Leído con éxito con valor1: %s\n", value1);
            } else if(res == -1) {
                printf("[LECTOR] ERROR al leer la clave.\n");
            }
            usleep(1000);
    }
    printf("[LECTOR] ¡Terminado!\n");
    return 0;
}