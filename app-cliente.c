#include "claves.h"
#include <stdio.h>



int main(){

     //SET_VALUE
    // Test 1. Añadir una nueva clave usando set_value

    // Preparamos los datos a enviar
    char *clave_test1 = "clave1";
    char test1_value1[256] = "test1_value1";
    int test1_N_value2 = 4;
    float test1_V_value2[4] = {1.5, 2.6, 3.4, 6.7};
    struct Paquete test1_paquete = {1,2,3};

    int test1_resultado = set_value(clave_test1,test1_value1,test1_N_value2,test1_V_value2,test1_paquete);

    printf("El resultado del test1 fue %d /RESULTADO ESPERADO 0\n\n", test1_resultado);

    char *clave_test2 = "clave1";
    char test2_value1[256] = "";
    int test2_N_value2 = -1;
    float test2_V_value2[4] = {-1, -1, -1, -1};
    struct Paquete test2_paquete = {-1,-1,-1};

    int test2_resultado = get_value(clave_test2,test2_value1,&test2_N_value2,test2_V_value2,&test2_paquete);

    printf("El resultado del test4 fue %d /RESULTADO ESPERADO 0\n", test2_resultado);
    if(test2_resultado == 0){
        printf("Información recuperada: clave: %s,value1: %s ,N_value2: %d, paquete x: %d y: %d z: %d\n","test1",test2_value1,test2_N_value2,test2_paquete.x,test2_paquete.y,test2_paquete.z);
        printf("El array V_value2 tiene los siguiente valores:\n");
        for(int i = 0; i < test2_N_value2; i++){
            printf("%f ", test2_V_value2[i]);
        }
    }
        printf("\n\n");

    return 0;
}