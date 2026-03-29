#include "claves.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("[LECTOR] Iniciando test de concurrencia...\n");
    
    char *key = "clave_compartida";
    char value1[256];
    int N_value2;
    float V_value2[32];
    struct Paquete paquete;

    for (int i = 0; i < 150; i++) {
        if (exist(key) == 1) {
            int res = get_value(key, value1, &N_value2, V_value2, &paquete);
            if (res == 0) {
                printf("[LECTOR] Leído con éxito: %s\n", value1);
            } else {
                printf("[LECTOR] ERROR al leer la clave.\n");
            }
        } else {
            printf("[LECTOR] La clave aún no existe, esperando...\n");
        }
        usleep(8000); // Lee un poco más rápido de lo que el otro escribe
    }
    printf("[LECTOR] ¡Terminado!\n");
    return 0;
}