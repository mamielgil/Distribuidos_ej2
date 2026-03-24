#include "claves.h"
#include <stdio.h>


int main(){

    if(delete_key("test1") == 0){
        printf("Se borró la key\n");

    }else{
        printf("No existe key\n");
    }
    return 0;

}