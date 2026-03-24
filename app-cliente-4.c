 #include "claves.h" 
 #include <stdio.h>
  
int main(){

    // Preparamos los datos a enviar
    char *clave_test1 = "test1";
    char test1_value1[256] = "test1_value1";
    int test1_N_value2 = 4;
    float test1_V_value2[4] = {5, 6, 7};
    struct Paquete test1_paquete = {45, 56, 67};

    int test1_resultado = set_value(clave_test1,test1_value1,test1_N_value2,test1_V_value2,test1_paquete);

    printf("El resultado del test1 fue %d /RESULTADO ESPERADO 0\n\n", test1_resultado);
    return 0;
}