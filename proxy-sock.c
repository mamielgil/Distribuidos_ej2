#include <mqueue.h>
#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAXSIZE 256

/*
CÓDIGOS DE OPERACIÓN:

0 -> DESTROY
1 -> SET_VALUE
2 -> GET_VALUE
3 -> MODIFY_VALUE
4 -> DELETE_KEY
5 -> EXIST
*/

struct peticion {
    // La información que conforma la petición.
    int codigo_operacion;
    char cola_cliente[MAXSIZE];
    char key[MAXSIZE];
    char value_1[MAXSIZE];
    float V_value2[32];
    int N_value2;
    struct Paquete value3;
};

struct respuesta {
  
    // Queremos que el usuario solamente sepa sobre el estado de la operación
    int resultado_operacion;
   
};


mqd_t cola_servidor;
mqd_t cola_cliente;
char nombre_cola[MAXSIZE];


int crear_cola(void){

    struct mq_attr atributos;

    atributos.mq_maxmsg = 1;
    atributos.mq_msgsize = sizeof(struct respuesta);

    sprintf(nombre_cola,"/cola_cliente-%d",getpid());

    cola_cliente = mq_open(nombre_cola,O_CREAT|O_RDONLY,0666,&atributos);

    if(cola_cliente == -1){
        perror("No se pudo abrir la cola del cliente");
        return -1;
    }

    cola_servidor = mq_open("/cola-server",O_WRONLY);

    if(cola_servidor == -1){
        perror("No se pudo abrir la cola del servidor");
        return -1;

    }

    return 0;
}


int cerrar_cola(void){

    mq_close(cola_servidor);
    mq_close(cola_cliente);

    return 0;


}

int destroy(){

    crear_cola();
    struct peticion pet;
    struct respuesta res;
    pet.codigo_operacion = 0;
    strcpy(pet.cola_cliente,nombre_cola);

    if(mq_send(cola_servidor,(const char *)&pet,sizeof(struct peticion),0) == -1){

        cerrar_cola();
        perror("No se ha podido enviar la peticion");
        return -2;
    }

    if(mq_receive(cola_cliente,(char *)&res,sizeof(struct respuesta),0) == -1){

        cerrar_cola();
        perror("No se ha podido recibir la respuesta del destroy");
        return -2;
    }

    cerrar_cola();
    return res.resultado_operacion;


}

int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){

    crear_cola();

    struct peticion pet;
    struct respuesta res;
    pet.codigo_operacion = 1;
    strcpy(pet.cola_cliente,nombre_cola);
    strcpy(pet.key,key);
    strcpy(pet.value_1,value1);
    if(N_value2 > 32 || N_value2 < 0){

        return -1;
    }
    pet.N_value2 = N_value2;
    memcpy(pet.V_value2, V_value2, sizeof(float) * N_value2);
    pet.value3 = value3;


    if(mq_send(cola_servidor,(const char *)&pet,sizeof(struct peticion),0) == -1){

        cerrar_cola();
        perror("No se ha podido enviar la peticion");
        return -2;
    }

    if(mq_receive(cola_cliente,(char *)&res,sizeof(struct respuesta),0) == -1){

        cerrar_cola();
        perror("No se ha podido recibir la respuesta del set_value");
        return -2;
    }

    cerrar_cola();
    return res.resultado_operacion;

}

int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3){

    crear_cola();

    struct peticion pet;
    struct respuesta res;
    pet.codigo_operacion = 2;
    strcpy(pet.cola_cliente,nombre_cola);
    strcpy(pet.key,key);

    if(mq_send(cola_servidor,(const char *)&pet,sizeof(struct peticion),0) == -1){

        cerrar_cola();
        perror("No se ha podido enviar la peticion");
        return -2;
    }

    if(mq_receive(cola_cliente,(char *)&res,sizeof(struct respuesta),0) == -1){

        cerrar_cola();
        perror("No se ha podido recibir la respuesta del get_value");
        return -2;
    }

    cerrar_cola();
    return res.resultado_operacion;


}

int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){

    crear_cola();

    struct peticion pet;
    struct respuesta res;
    pet.codigo_operacion = 3;
    strcpy(pet.cola_cliente,nombre_cola);
    strcpy(pet.key,key);
    strcpy(pet.value_1,value1);
    pet.N_value2 = N_value2;
    if(N_value2 > 32 || N_value2 < 0){
        return -1;
    }
    memcpy(pet.V_value2, V_value2, sizeof(float) * N_value2);
    pet.value3 = value3;


    if(mq_send(cola_servidor,(const char *)&pet,sizeof(struct peticion),0) == -1){

        cerrar_cola();
        perror("No se ha podido enviar la peticion");
        return -2;
    }

    if(mq_receive(cola_cliente,(char *)&res,sizeof(struct respuesta),0) == -1){

        cerrar_cola();
        perror("No se ha podido recibir la respuesta del modify_value");
        return -2;
    }

    cerrar_cola();
    return res.resultado_operacion;

}


int delete_key(char *key){

    crear_cola();

    struct peticion pet;
    struct respuesta res;
    pet.codigo_operacion = 4;
    strcpy(pet.cola_cliente,nombre_cola);
    strcpy(pet.key,key);
    

    if(mq_send(cola_servidor,(const char *)&pet,sizeof(struct peticion),0) == -1){

        cerrar_cola();
        perror("No se ha podido enviar la peticion");
        return -2;
    }

    if(mq_receive(cola_cliente,(char *)&res,sizeof(struct respuesta),0) == -1){

        cerrar_cola();
        perror("No se ha podido recibir la respuesta del delete_key");
        return -2;
    }

    cerrar_cola();
    return res.resultado_operacion;

}


int exist(char *key){

    crear_cola();

    struct peticion pet;
    struct respuesta res;
    pet.codigo_operacion = 5;
    strcpy(pet.cola_cliente,nombre_cola);
    strcpy(pet.key,key);
    

    if(mq_send(cola_servidor,(const char *)&pet,sizeof(struct peticion),0) == -1){

        cerrar_cola();
        perror("No se ha podido enviar la peticion");
        return -2;
    }

    if(mq_receive(cola_cliente,(char *)&res,sizeof(struct respuesta),0) == -1){

        cerrar_cola();
        perror("No se ha podido recibir la respuesta del delete_key");
        return -2;
    }

    cerrar_cola();
    return res.resultado_operacion;

}


