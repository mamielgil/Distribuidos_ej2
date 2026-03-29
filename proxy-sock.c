#include "lines.h"
#include <stdlib.h>
#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <netinet/in.h>

// Seleccionamos un tamaño grande para poder almacenar 
// el vector V_value2
# define MAX_V_VALUE2_SIZE 1024

/*
CÓDIGOS DE OPERACIÓN:

0 -> DESTROY
1 -> SET_VALUE
2 -> GET_VALUE
3 -> MODIFY_VALUE
4 -> DELETE_KEY
5 -> EXIST
*/


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
    char cod_operacion[3] = "0";

    char respuesta[3] = "";

    //Mandamos la instrucción a realizar
    if(sendMessage(sd,cod_operacion,strlen(cod_operacion) + 1) == -1){
        close(sd);
        return -2;
    }

    // Esperamos a la obtención de la respuesta
    if(readLine(sd,respuesta,3) < 0){
        close(sd);
        return -2;
    }

    // Cerramos el socket
    close(sd);

    // Devolvemos el código de resultado
    return atoi(respuesta);
}

int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
    // Enviamos todos los parámetros
    int sd = crear_conexion_server();

    if(sd == -1){
        // Hubo algún error durante la creación de la conexión
        return -2;
    }

    char cod_operacion[3] = "1";

    //Enviamos el código de operación
    if(sendMessage(sd,cod_operacion, strlen(cod_operacion) + 1) == -1){
        close(sd);
        return -2;
    }

    // Enviamos la key
    if(sendMessage(sd,key,strlen(key) + 1) == -1){
        close(sd);
        return -2;
    }

    // Enviamos el valor 1

    if(sendMessage(sd,value1,strlen(value1) + 1) == -1){
        close(sd);
        return -2;
    }

    // Enviamos el N_value2
    if(N_value2 > 32 || N_value2 <= 0){
        // El error de N_value2 es incorrecto
        close(sd);
        return -2;
    }
    char buffer[MAX_V_VALUE2_SIZE];
    strcpy(buffer,"");

    // Guardamos el número en el buffer
    sprintf(buffer,"%d",N_value2);

    sendMessage(sd, buffer, strlen(buffer) + 1);

    // Enviamos V_value2( enviamos tantos floats como indica N_value2)

    strcpy(buffer, "[");

    for(int i = 0; i < N_value2; i++){
        char tmp[20];
        strcpy(tmp,"");
        if(i != N_value2 - 1){
            // Si no es la última iteración, añadimos el número separado por una coma
            sprintf(tmp, "%f,",V_value2[i]);
        }else{
            sprintf(tmp, "%f]",V_value2[i]);
        }
        strcat(buffer, tmp);
    }

    // Una vez ya obtenido el buffer, lo enviamos

    sendMessage(sd,buffer, strlen(buffer) + 1);

    // Por último enviamos value3 en formato de array de tres dígitos

    strcpy(buffer, "");

    sprintf(buffer,"[%d,%d,%d]",value3.x,value3.y,value3.z);

    sendMessage(sd,buffer, strlen(buffer) + 1);

    // Una vez enviado todo, esperamos a la respuesta del servidor

    strcpy(buffer, "");

    if(readLine(sd,buffer,3) < 0){
        close(sd);
        return -2;
    }

    // Devolvemos el código de ejecución
    close(sd);
    return atoi(buffer);

}

int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3){
    
    // En el caso del get, solamente enviamos el código de operación y la key y recibimos los valores
    int sd = crear_conexion_server();
    if(sd == -1){
        // Hubo algún error durante la creación del socket
        return -2;
    }

    char cod_operacion[3] = "2";

    //Enviamos el código de operación
    if(sendMessage(sd,cod_operacion, strlen(cod_operacion) + 1) == -1){
        close(sd);
        return -2;
    }

    // Enviamos la key
    if(sendMessage(sd,key,strlen(key) + 1) == -1){
        close(sd);
        return -2;
    }

    // Ahora recibimos el código de ejecución y después los valores recuperados con el get

    char cod_res[3];
    char buffer[MAX_V_VALUE2_SIZE];
    strcpy(buffer,"");
    strcpy(cod_res,"");

    // Recibimos el código de ejecución
    if(readLine(sd,cod_res,3) < 0){
        close(sd);
        return -2;
    }

    if(atoi(cod_res) != 0){
        // No se pudo realizar el get por lo que no debemos leer el resto de valores
        close(sd);
        return atoi(cod_res);
    }

    // Recibimos valores del get

    // Recibimos el valor 1
    if(readLine(sd,buffer,MAX_V_VALUE2_SIZE - 1) < 0){
        close(sd);
        return -2;
    }

    strcpy(value1,buffer);

    strcpy(buffer,"");

    // Recibimos N_value2

    if(readLine(sd,buffer,MAX_V_VALUE2_SIZE - 1) < 0){
        close(sd);
        return -2;
    }

    // Guardamos el N_value2 obtenido
    *N_value2 = atoi(buffer);

    // Recibimos ahora V_value2

    strcpy(buffer, "");
    
    if(readLine(sd,buffer,MAX_V_VALUE2_SIZE - 1) < 0){
        close(sd);
        return -2;
    }

    char *tokens = strtok(buffer,"[],");
    int i = 0;

    // Lo copiamos al array dado
    while(tokens != NULL && i < *N_value2){

        V_value2[i] = strtof(tokens, NULL);

        // Obtenemos el siguiente número del array
        tokens = strtok(NULL, "[],");
        // Aumentamos la posición de i
        i++;
    }

    // Por último recibimos el paquete

    strcpy(buffer,"");

    if(readLine(sd,buffer,MAX_V_VALUE2_SIZE - 1) < 0){
        close(sd);
        return -2;
    }

    tokens = strtok(buffer, "[],");

    if(tokens == NULL){
        close(sd);
        return -2;
    }

    value3->x = atoi(tokens);

    tokens = strtok(NULL, "[],");
    if(tokens == NULL){
        close(sd);
        return -2;
    }
    value3->y = atoi(tokens);

    tokens = strtok(NULL, "[],");
    if(tokens == NULL){
        close(sd);
        return -2;
    }
    value3->z = atoi(tokens);

    // Ya están obtenidos todos los valores por lo que finalizamos la conexión
    close(sd);
    return atoi(cod_res);

}

int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
     
    // Enviamos todos los parámetros
    int sd = crear_conexion_server();

    if(sd == -1){
        // Hubo algún error durante la creación de la conexión
        return -2;
    }

    char cod_operacion[3] = "3";

    //Enviamos el código de operación
    if(sendMessage(sd,cod_operacion, strlen(cod_operacion) + 1) == -1){
        close(sd);
        return -2;
    }

    // Enviamos la key
    if(sendMessage(sd,key,strlen(key) + 1) == -1){
        close(sd);
        return -2;
    }

    // Enviamos el valor 1

    if(sendMessage(sd,value1,strlen(value1) + 1) == -1){
        close(sd);
        return -2;
    }

    // Enviamos el N_value2
    if(N_value2 > 32 || N_value2 <= 0){
        // El error de N_value2 es incorrecto
        close(sd);
        return -2;
    }
    char buffer[MAX_V_VALUE2_SIZE];
    strcpy(buffer,"");

    // Guardamos el número en el buffer
    sprintf(buffer,"%d",N_value2);

    sendMessage(sd, buffer, strlen(buffer) + 1);

    // Enviamos V_value2( enviamos tantos floats como indica N_value2)

    strcpy(buffer, "[");

    for(int i = 0; i < N_value2; i++){
        char tmp[20];
        strcpy(tmp,"");
        if(i != N_value2 - 1){
            // Si no es la última iteración, añadimos el número separado por una coma
            sprintf(tmp, "%f,",V_value2[i]);
        }else{
            sprintf(tmp, "%f]",V_value2[i]);
        }
        strcat(buffer, tmp);
    }

    // Una vez ya obtenido el buffer, lo enviamos

    sendMessage(sd,buffer, strlen(buffer) + 1);

    // Por último enviamos value3 en formato de array de tres dígitos

    strcpy(buffer, "");

    sprintf(buffer,"[%d,%d,%d]",value3.x,value3.y,value3.z);

    sendMessage(sd,buffer, strlen(buffer) + 1);

    // Una vez enviado todo, esperamos a la respuesta del servidor

    strcpy(buffer, "");

    if(readLine(sd,buffer,3) < 0){
        close(sd);
        return -2;
    }

    // Devolvemos el código de ejecución
    close(sd);
    return atoi(buffer);

}


int delete_key(char *key){
    // En el delete_key solamente necesitamos enviar la key

    int sd = crear_conexion_server();
    if(sd == -1){
        return -2;
    }

    // Ahora enviamos el código de operación
    char cod_op[3] = "4"; 
    sendMessage(sd,cod_op,3);

    // Ahora enviamos la key a borrar
    char buffer[MAX_V_VALUE2_SIZE];
    strcpy(buffer,key);
    sendMessage(sd,buffer,strlen(buffer) + 1);

    // Una vez enviada la key, recibimos el código de ejecucion del server
    strcpy(buffer, "");
    
    if(readLine(sd,buffer,3) < 0){
        // Hubo un error en la obtención del resultado
        close(sd);
        return -2;
    }

    close(sd);
    // Devolvemos el resultado
    return atoi(buffer);

}


int exist(char *key){
    // En el exist solamente necesitamos enviar la key

    int sd = crear_conexion_server();
    if(sd == -1){
        return -2;
    }

    // Ahora enviamos el código de operación
    char cod_op[3] = "5"; 
    sendMessage(sd,cod_op,3);

    // Ahora enviamos la key a borrar
    char buffer[MAX_V_VALUE2_SIZE];
    strcpy(buffer,key);
    sendMessage(sd,buffer,strlen(buffer) + 1);

    // Una vez enviada la key, recibimos el código de ejecucion del server
    strcpy(buffer, "");
    
    if(readLine(sd,buffer,3) < 0){
        // Hubo un error en la obtención del resultado
        close(sd);
        return -2;
    }

    close(sd);
    // Devolvemos el resultado
    return atoi(buffer);
}


