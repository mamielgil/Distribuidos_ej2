 #include "claves.h" 
 #include <stdio.h>
  
int main(){
  

    // Preparamos los datos a enviar
    char *clave_test1 = "test1";
    char test1_value1[256] = "test1_value1";
    int test1_N_value2 = 4;
    float test1_V_value2[4] = {1.5, 2.6, 3.4, 6.7};
    struct Paquete test1_paquete = {1,2,3};

    int test1_resultado = get_value(clave_test1,test1_value1,&test1_N_value2,test1_V_value2,&test1_paquete);

    if(test1_resultado == 0){
        printf("Se recuperaron los resultados!: value1 %s, N_value2 %d, paquete{%d, %d, %d}\n",test1_value1,test1_N_value2,test1_paquete.x,test1_paquete.y,test1_paquete.z);
    }else{
        printf("No se pudo recuperar la info\n");
    }
    return 0;
}