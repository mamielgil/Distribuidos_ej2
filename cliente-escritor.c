#include "claves.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    printf("[ESCRITOR] Iniciando test de concurrencia...\n");
    
    char *key = "clave_compartida";
    char value1[256];
    float V_value2[4] = {1.1, 2.2, 3.3, 4.4};
    struct Paquete paquete = {10, 20, 30};

    for (int i = 0; i < 100; i++) {
        sprintf(value1, "Escritura_numero_%d", i);
        
        // Alternamos entre set y modify
        if (i % 2 == 0) {
            set_value(key, value1, 4, V_value2, paquete);
            printf("[ESCRITOR] SET: %s\n", value1);
        } else {
            modify_value(key, value1, 4, V_value2, paquete);
            printf("[ESCRITOR] MODIFY: %s\n", value1);
        }
        
        // Pausa de 10 milisegundos para permitir que otros hilos entren
        usleep(10000); 
    }
    printf("[ESCRITOR] ¡Terminado!\n");
    return 0;
}