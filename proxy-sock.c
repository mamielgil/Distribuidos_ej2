#include <stdlib.h>
#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <netinet/in.h>
#include <lines.h>
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



int obtener_credenciales_server(char **server_ip,char **puerto_server){

    // Obtenemos la IP y el puerto del servidor
    char *server_ip_env = getenv("IP_TUPLAS");

    char *server_puerto_env = getenv("PORT_TUPLAS");


    if(server_ip_env != NULL && server_puerto_env != NULL){
        
        *server_ip = server_ip_env;

        // Convertimos el puerto a número
        *puerto_server = server_puerto_env;
        return 0;
    }

    return -1;
}

int crear_conexion_server(){
    char *ip_server;
    char *puerto_server;

    if(obtener_credenciales_server(&ip_server, &puerto_server) != 0){
        printf("No se pudieron obtener las credenciales del servidor\n");
        // CODIGO DE ERROR AL NO ENCONTRAR CREDENCIALES DEL SERVER
        return -1;
    }

    // Creamos el socket
   int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if(sd == -1){
    printf("El socket no se pudo crear correctamente\n");
    return -1;
   }

    // Ahora podemos comenzar a establecer la conexion

   struct hostent *host;
   struct sockaddr_in server_addr;

   bzero((char *) &server_addr, sizeof(server_addr));

   server_addr.sin_family = AF_INET;    
   server_addr.sin_port = htons(atoi(puerto_server));

   // Con inet_aton podemos comprobar si la notacion es decimal o dominio-punto

   if(inet_aton(ip_server,&server_addr.sin_addr) == 0){
        // inet_aton devuelve 0 si la dirección no es formato decimal punto, intentamos dominio punto
        host = gethostbyname(ip_server);

        if(host == NULL){
            printf("Error al obtener la dirección del servidor\n");
            close(sd);
            return -1;
        }
        // Guardamos la dirección obtenida
        memcpy(&server_addr.sin_addr,host->h_addr_list[0],host->h_length);
   }


   if(connect(sd,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
    printf("Error en la conexión\n");
    close(sd);
    return -1;

   }

   return sd;
}


int destroy(){

    int sd = crear_conexion_server();
    if( sd == -1){
        // Hubo algún error durante la creación del socket
        return -2;
    }

    // Enviamos los datos al servidor para que realize su funcion
    char cod_operacion[2] = "0";

    char respuesta[3] = "";

    //Mandamos la instrucción a realizar
    if(sendMessage(sd,cod_operacion,strlen(cod_operacion) + 1) == -1){
        close(sd);
        return -2;
    }

    // Esperamos a la obtención de la respuesta
    if(readLine(sd,respuesta,3) <= 0){
        close(sd);
        return -2;
    }

    // Cerramos el socket
    close(sd);

    // Devolvemos el código de resultado
    return atoi(respuesta);
}

int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){


}

int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3){

    

}

int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){


}


int delete_key(char *key){

    

}


int exist(char *key){

    

}


