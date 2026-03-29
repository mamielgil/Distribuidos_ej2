#include "claves.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("[CAOS] Iniciando test de concurrencia...\n");
    
    char value1[256] = "basura";
    float V_value2[1] = {0.0};
    struct Paquete paquete = {0, 0, 0};

    for (int i = 0; i < 50; i++) {
        // 1. Crea y borra una clave propia muy rápido
        set_value("clave_efimera", value1, 1, V_value2, paquete);
        printf("[CAOS] Creada clave efímera.\n");
        
        usleep(5000);
        
        delete_key("clave_efimera");
        printf("[CAOS] Borrada clave efímera.\n");

        // 2. De vez en cuando, intenta borrar la compartida para molestar al lector y escritor
        if (i % 10 == 0) {
            printf("[CAOS] ¡Intentando borrar la clave compartida!\n");
            delete_key("clave_compartida");
        }
        
        usleep(15000);
    }
    printf("[CAOS] ¡Terminado!\n");
    return 0;
}