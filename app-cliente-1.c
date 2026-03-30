#include "claves.h"
#include <stdio.h>
#include <string.h>


// EN ESTE APP-CLIENTE SE COMPRUEBAN QUE LAS LLAMADAS FUNCIONEN LÓGICAMENTE, 
// ES DECIR ESTE PROGRAMA SE EJECUTA SIN NINGUN OTRO CLIENTE


void tests1_3(){
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

    // Test 2. Recuperamos la clave anteriormente creada
    char *clave_test2 = "clave1";
    char test2_value1[256] = "";
    int test2_N_value2 = -1;
    float test2_V_value2[4] = {-1, -1, -1, -1};
    struct Paquete test2_paquete = {-1,-1,-1};

    int test2_resultado = get_value(clave_test2,test2_value1,&test2_N_value2,test2_V_value2,&test2_paquete);

    printf("El resultado del test2 fue %d /RESULTADO ESPERADO 0\n", test2_resultado);
    int test2_valid = 0;
    if(test2_resultado == 0){

        if(strcmp(test2_value1, test1_value1) != 0){
            printf("[ERROR] Test 2: El value1 recuperado no coincide.\n");
            test2_valid = -1;
        }

        if(test2_N_value2 != test1_N_value2){
          printf("[ERROR] Test 2: El N_value2 recuperado no coincide.\n");
           test2_valid = -1;
        }

        for(int i = 0; i < test2_N_value2; i++){
            if(test2_V_value2[i] != test1_V_value2[i]){
                printf("[ERROR] Test 2: El V_value2 recuperado no coincide.\n");
                 test2_valid = -1;
            }
        }
        if(test1_paquete.x != test2_paquete.x || test1_paquete.y != test2_paquete.y || test1_paquete.z != test2_paquete.z){
            printf("[ERROR] Test 2: El value3 recuperado no coincide.\n");
            test2_valid = -1;
        }
        
    }
    if(test2_resultado != 0 || test2_valid == -1) {
        printf("[ERROR] Test 2 falló. Esperado 0, obtenido %d\n", test2_resultado);
    } else {
        printf("[OK] Test 2 superado.\n");
    }
    printf("\n");

    //Test 3. Intentamos hacer set sobre la clave anterior de forma que al existir
    // se espera un error

     // Preparamos los datos a enviar
    char *clave_test3 = "clave1";
    char test3_value1[256] = "";
    int test3_N_value2 = -1;
    float test3_V_value2[4] = {-1, -1, -1, -1};
    struct Paquete test3_paquete = {-1,-1,-1};

    int test3_resultado = set_value(clave_test3,test3_value1,test3_N_value2,test3_V_value2,test3_paquete);
    printf("El resultado del test3 fue %d /RESULTADO ESPERADO -1\n", test3_resultado);

}

void tests4_8(){

    // Test 4. Modificamos la clave anterior(clave1)
    char *clave_test4 = "clave1";
    char test4_value1[256] = "TEST4";
    int test4_N_value2 = 4;
    float test4_V_value2[4] = {4.1, 4.2, 4.3, 4.4};
    struct Paquete test4_paquete = {4.5, 4.6, 4.7};

    // Modificamos los valores
    int test4_resultado = modify_value(clave_test4,test4_value1,test4_N_value2,test4_V_value2,test4_paquete);
    printf("El resultado del test4 fue %d /RESULTADO ESPERADO 0\n", test4_resultado);


    // Test 5. Obtenemos los datos de la clave para asegurarnos que se ha cambiado
    char *clave_test5 = "clave1";
    char test5_value1[256] = "";
    int test5_N_value2 = -1;
    float test5_V_value2[4] = {-1, -1, -1, -1};
    struct Paquete test5_paquete = {-1,-1,-1};

    int test5_resultado = get_value(clave_test5,test5_value1,&test5_N_value2,test5_V_value2,&test5_paquete);
    printf("El resultado del test5 fue %d /RESULTADO ESPERADO 0\n", test5_resultado);
    int test5_valid = 0;
    if(test5_resultado == 0){

        if(strcmp(test5_value1, test4_value1) != 0){
            printf("[ERROR] Test 5: El value1 recuperado no coincide.\n");
            test5_valid = -1;
        }

        if(test5_N_value2 != test4_N_value2){
          printf("[ERROR] Test 5: El N_value2 recuperado no coincide.\n");
           test5_valid = -1;
        }

        for(int i = 0; i < test5_N_value2; i++){
            if(test5_V_value2[i] != test4_V_value2[i]){
                printf("[ERROR] Test 5: El V_value2 recuperado no coincide.\n");
                 test5_valid = -1;
            }
        }
        if(test5_paquete.x != test4_paquete.x || test5_paquete.y != test4_paquete.y || test5_paquete.z != test4_paquete.z){
            printf("[ERROR] Test 5: El value3 recuperado no coincide.\n");
            test5_valid = -1;
        }
        
    }
    if(test5_resultado != 0 || test5_valid == -1) {
        printf("[ERROR] Test 5 falló. Esperado 0, obtenido %d\n", test5_resultado);
    } else {
        printf("[OK] Test 5 superado.\n");
    }
    printf("\n");


    // Test 6. Intentamos modificar una clave que no existe
    char *clave_test6 = "no_existo";
    char test6_value1[256] = "inexistencia";
    int test6_N_value2 = 5;
    float test6_V_value2[4] = {9, 1, 0, 0};
    struct Paquete test6_paquete = {0, 56, 456};

    int test6_resultado = modify_value(clave_test6, test6_value1, test6_N_value2, test6_V_value2, test6_paquete);
    printf("El resultado del test6 fue %d /RESULTADO ESPERADO -1\n", test6_resultado);


    // Test 7. Intentamos modificar una clave podiendo un N_value2 fuera de rango
    char *clave_test7 = "clave1";
    char test7_value1[256] = "no_deberia_estar";
    int test7_N_value2 = 33;
    float test7_V_value2[4] = {9, 1, 0, 0};
    struct Paquete test7_paquete = {-16, 67, 91};

    int test7_resultado = modify_value(clave_test7, test7_value1, test7_N_value2, test7_V_value2, test7_paquete);
    printf("El resultado del test7 fue %d /RESULTADO ESPERADO -1\n", test7_resultado);

    // Test 8. Intentamos obtener los datos de una clave que no existe

    char *clave_test8 = "no_existo";
    char test8_value1[256] = "inexistencia";
    int test8_N_value2 = 5;
    float test8_V_value2[4] = {9, 1, 0, 0};
    struct Paquete test8_paquete = {0, 56, 456};

    int test8_resultado = get_value(clave_test8, test8_value1, &test8_N_value2, test8_V_value2, &test8_paquete);
    printf("El resultado del test8 fue %d /RESULTADO ESPERADO -1\n", test8_resultado);

}

void tests9_12(){

    // Test 9. Comprobamos que la key clave 1 existe

    char *clave_test9 = "clave1";
    int test9_resultado = exist(clave_test9);
    printf("El resultado del test9 fue %d /RESULTADO ESPERADO 1\n", test9_resultado);

    // Test 10. Comprobamos una key inexistente

    char *clave_test10 = "no_existo";
    int test10_resultado = exist(clave_test10);
    printf("El resultado del test10 fue %d /RESULTADO ESPERADO 0\n", test10_resultado);

    // Test 11. Borramos la key clave 1

    char *clave_test11 = "clave1";
    int test11_resultado = delete_key(clave_test11);
    printf("El resultado del test11 fue %d /RESULTADO ESPERADO 0\n", test11_resultado);

    // Test 12. Intentamos borrar la key clave 1 de nuevo(como ya no existe se espera error)
    int test12_resultado = delete_key(clave_test11);
    printf("El resultado del test12 fue %d /RESULTADO ESPERADO -1\n", test12_resultado);

}


void test13(){
    
    // Test 13. Creamos una serie de keys y nos aseguramos que se hayan borrado

    char variable_key[10] = "clave1";
    char test13_value1[256] = "test1_value1";
    int test13_N_value2 = 4;
    float test13_V_value2[4] = {1.5, 2.6, 3.4, 6.7};
    struct Paquete test13_paquete = {1,2,3};


    for(int i = 2; i < 12; i++){
        int resultado = set_value(variable_key,test13_value1,test13_N_value2,test13_V_value2,test13_paquete);
        printf("La key: %s al ejecutar el set dio resultado(0 CREADA/ -1 NO CREADA) %d\n", variable_key, resultado);

        sprintf(variable_key,"clave%d",i);
        
    }

    int test13_resultado = destroy();
     printf("El resultado del test13 fue %d /RESULTADO ESPERADO 0\n", test13_resultado);


}



int main(){

    // Ejecutamos los tests definidos en la parte superior
    tests1_3();
    tests4_8();
    tests9_12();
    test13();
    

    return 0;
}
