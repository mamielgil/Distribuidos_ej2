 #include "claves.h" 
 #include <stdio.h>
  
int main(){


    // Preparamos los datos a enviar
    char *clave_test1 = "test1";
    char test1_value1[256] = "modificando";
    int test1_N_value2 = 1;
    float test1_V_value2[4] = {5};
    struct Paquete test1_paquete = {0,0,9};

    int test1_resultado = modify_value(clave_test1,test1_value1,test1_N_value2,test1_V_value2,test1_paquete);

    if(test1_resultado == 0){
        printf("Se modificaron la clave!\n");
    }else{
        printf("No se pudo modificar la clave la info\n");
    }
    return 0;
}