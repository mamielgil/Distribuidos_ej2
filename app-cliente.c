#include "claves.h"
#include <stdio.h>

void set_value_tests();
void get_value_tests();
void modify_value_tests();
void exist_value_tests();
void delete_value_tests();
void destroy_value_tests();

int main(){

    set_value_tests();

    get_value_tests();

    modify_value_tests();

    exist_value_tests();

    delete_value_tests();

    destroy_value_tests();

    return 0;
}


void set_value_tests(){

    //SET_VALUE
    // Test 1. Añadir una nueva clave usando set_value

    // Preparamos los datos a enviar
    char *clave_test1 = "test1";
    char test1_value1[256] = "test1_value1";
    int test1_N_value2 = 4;
    float test1_V_value2[4] = {1.5, 2.6, 3.4, 6.7};
    struct Paquete test1_paquete = {1,2,3};

    int test1_resultado = set_value(clave_test1,test1_value1,test1_N_value2,test1_V_value2,test1_paquete);

    printf("El resultado del test1 fue %d /RESULTADO ESPERADO 0\n\n", test1_resultado);


    // Test 2. Intentar añadir una clave repetida
    // Preparamos los datos a enviar
    char *clave_test2 = "test1";
    char test2_value1[256] = "test2_value1";
    int test2_N_value2 = 3;
    float test2_V_value2[3] = {1.5, 5.86, 5.6};
    struct Paquete test2_paquete = {1,2,3};

    int test2_resultado = set_value(clave_test2,test2_value1,test2_N_value2,test2_V_value2,test2_paquete);

    printf("El resultado del test2 fue %d /RESULTADO ESPERADO -1\n\n", test2_resultado);

    // Test 3. Intentar añadir una clave de forma que N_value 2 esté fuera de rango

    char *clave_test3 = "test3";
    char test3_value1[256] = "test3_value1";
    int test3_N_value2 = 33;
    float test3_V_value2[3] = {1.5, 5.86, 5.6};
    struct Paquete test3_paquete = {1,2,3};

    int test3_resultado = set_value(clave_test3,test3_value1,test3_N_value2,test3_V_value2,test3_paquete);

    printf("El resultado del test3 fue %d /RESULTADO ESPERADO -1\n\n", test3_resultado);



}

void get_value_tests(){

    // GET_VALUE
    // Test 4. Recuperar correctamente los datos de una clave concreta
    char test4_value1[256] = "";
    int test4_N_value2 = 0;
    float test4_V_value2[32] = {};
    struct Paquete test4_paquete = {};
    int test4_resultado = get_value("test1",test4_value1,&test4_N_value2,test4_V_value2,&test4_paquete);

    printf("El resultado del test4 fue %d /RESULTADO ESPERADO 0\n", test4_resultado);
    printf("Información recuperada: clave: %s,value1: %s ,N_value2: %d, paquete x: %d y: %d z: %d\n","test1",test4_value1,test4_N_value2,test4_paquete.x,test4_paquete.y,test4_paquete.z);
    printf("El array V_value2 tiene los siguiente valores:\n");
    for(int i = 0; i < test4_N_value2; i++){
        printf("%f ", test4_V_value2[i]);
    }
    printf("\n\n");

    // Test 5. Intentar recuperar una clave no existente

    char test5_value1[256] = "";
    int test5_N_value2 = 0;
    float test5_V_value2[32] = {};
    struct Paquete test5_paquete = {};
    int test5_resultado = get_value("no_existo",test5_value1,&test5_N_value2,test5_V_value2,&test5_paquete);

    printf("El resultado del test5 fue %d /RESULTADO ESPERADO -1\n\n", test5_resultado);

}


void modify_value_tests(){

    //MODIFY_VALUE
    // Test 6. Modificar correctamente una clave existente

    char *clave_test1 = "test1";
    char test1_value1[256] = "test1_value1";
    int test1_N_value2 = 4;
    float test1_V_value2[4] = {1.5, 2.6, 3.4, 6.7};
    struct Paquete test1_paquete = {1,2,3};


    char *clave_test6 = "test1";
    char test6_value1[256] = "a";
    int test6_N_value2 = 5;
    float test6_V_value2[5] = {2, 3, 4, 5, 6};
    struct Paquete test6_paquete = {100,200,300};

    printf("Información actual: clave: %s, value1: %s,N_value2: %d, paquete x: %d y: %d z: %d\n",clave_test1,test1_value1,test1_N_value2,test1_paquete.x,test1_paquete.y,test1_paquete.z);
    printf("El array V_value2 tiene los siguiente valores:\n");
    for(int i = 0; i < test1_N_value2; i++){
        printf("%f ", test1_V_value2[i]);
    }
    printf("\n\n");

    // Modificamos los valores

    int test6_resultado = modify_value(clave_test6, test6_value1, test6_N_value2, test6_V_value2, test6_paquete);

    printf("El resultado del test6 fue %d /RESULTADO ESPERADO 0\n", test6_resultado);

    get_value(clave_test6, test6_value1, &test6_N_value2, test6_V_value2, &test6_paquete);

    printf("Información actual: clave: %s, value1: %s,N_value2: %d, paquete x: %d y: %d z: %d\n",clave_test6,test6_value1,test6_N_value2,test6_paquete.x,test6_paquete.y,test6_paquete.z);
    printf("El array V_value2 tiene los siguiente valores:\n");
    for(int i = 0; i < test6_N_value2; i++){
        printf("%f ", test6_V_value2[i]);
    }
    printf("\n\n");

    // Test 7. Modificar una clave no existente

    char test7_value1[256] = "test7_value1";
    int test7_N_value2 = 4;
    float test7_V_value2[4] = {1.5, 2.6, 3.4, 6.7};
    struct Paquete test7_paquete = {1,2,3};

    int test7_resultado = modify_value("no_existo",test7_value1,test7_N_value2,test7_V_value2,test7_paquete);

    printf("El resultado del test7 fue %d /RESULTADO ESPERADO -1\n\n", test7_resultado);


    // Test 8. Intentar modificar una clave de forma que N_value 2 esté fuera de rango
    char *test8_clave = "test1";
    char test8_value1[256] = "test8_value1";
    int test8_N_value2 = 33;
    float test8_V_value2[33] = {1.5, 2.6, 3.4, 6.7};
    struct Paquete test8_paquete = {1,2,3};

    int test8_resultado = modify_value(test8_clave,test8_value1,test8_N_value2,test8_V_value2,test8_paquete);

    printf("El resultado del test8 fue %d /RESULTADO ESPERADO -1\n\n", test8_resultado);


}


void exist_value_tests(){

    //EXIST
    // Test 9. Comprobar la existencia de una clave de forma exitosa
    int test9_resultado = exist("test1");

    printf("El resultado del test9 fue %d /RESULTADO ESPERADO 1\n", test9_resultado);

    // Test 10. Intentar comprobar la existencia de una clave no presente
     int test10_resultado = exist("no_existo");

    printf("El resultado del test10 fue %d /RESULTADO ESPERADO 0\n\n", test10_resultado);

}

void delete_value_tests(){
    // DELETE_KEY
    // Test 11. Borrar correctamente un elemento con la key específicada
    // Preparamos los datos a enviar
    char *clave_test11 = "test1";

    int test11_resultado = delete_key(clave_test11);

    printf("El resultado del test11 fue %d /RESULTADO ESPERADO 0\n\n", test11_resultado);



// Test 12. Intentar borrar una clave no existente
    // Por ejemplo, podemos intentar borrar de nuevo la clave test1

    // Preparamos los datos a enviar
    char *clave_test12 = "test1";

    int test12_resultado = delete_key(clave_test12);

    printf("El resultado del test12 fue %d /RESULTADO ESPERADO -1\n\n", test12_resultado);


}

void destroy_value_tests(){
    //DESTROY
    // Test 13. Borrar todas las tuplas
    int test13_resultado =  destroy();

    printf("El resultado del test13 fue %d /RESULTADO ESPERADO 0\n\n", test13_resultado);

}