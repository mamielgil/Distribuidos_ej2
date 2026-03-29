#include "claves.h"
#include <fcntl.h>        
#include <sys/stat.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <lines.h>
#include <string.h>
#include <stdlib.h>

// Consideramos el defecto de Linux
# define NUM_MENSAJES 10
# define NUM_THREADS 50
# define MAX_SIZE 256
// Asumimos que cada número como máximo va a tener 10 dígitos
# define MAX_V_VALUE2_SIZE 321
/*
CÓDIGOS DE OPERACIÓN:

0 -> DESTROY
1 -> SET_VALUE
2 -> GET_VALUE
3 -> MODIFY_VALUE
4 -> DELETE_KEY
5 -> EXIST

// -2 es un código de error para operaciones no definidas
*/

struct peticion {
    // La información que conforma la petición.
    int codigo_operacion;
    char key[MAX_SIZE];
    char value_1[MAX_SIZE];
    float V_value2[32];
    int N_value2;
    struct Paquete value3;
};

struct respuesta {
  
    // Queremos que el usuario solamente sepa sobre el estado de la operación
    int resultado_operacion;
   
};

int buffer_socket[NUM_THREADS];
int n_elementos = 0;
int pos_servicio = 0;
pthread_mutex_t mi_mutex;
pthread_cond_t no_lleno;
pthread_cond_t no_vacio;
pthread_mutex_t mfin;
int fin = 0;

// Declaraciones para poder referenciar las funciones sin haber sido desarrolladas
int obtener_params(int sd, struct peticion *pet);
void enviar_datos_get(int sd, struct peticion mi_peticion);

void* tratar_peticion(void* peticion_cliente){
    struct peticion mi_peticion;
    struct respuesta respuesta_a_enviar;

    
    while(1){
        pthread_mutex_lock(&mi_mutex);

        while(n_elementos == 0){

            // En este caso, si ya no hay más peticiones que recibir y el thread se quedó
            // esperando, hacemos que finalice su ejecución
            if(fin == 1){
                printf("Finalizando el servicio\n");
                pthread_mutex_unlock(&mi_mutex);
                pthread_exit(0);
            }

            // Si no, hacemos que espere hasta que haya una petición disponible
            pthread_cond_wait(&no_vacio,&mi_mutex);
        }

        int sd = buffer_socket[pos_servicio];
        pos_servicio = (pos_servicio + 1) % NUM_THREADS;
        n_elementos--;
        pthread_cond_signal(&no_lleno);
        pthread_mutex_unlock(&mi_mutex);

        // Obtenemos los parámetros de la petición
        if(obtener_params(sd, &mi_peticion) == -1){
            // Este cliente no realiza su ejecución y se salta la petición
            close(sd);
            continue;
        }


        // Aquí ya procesamos la petición

        switch(mi_peticion.codigo_operacion){

            case 0:

                // Caso de destroy
                respuesta_a_enviar.resultado_operacion = destroy();
                break;

            case 1:

                // Caso set_value
                respuesta_a_enviar.resultado_operacion = set_value(mi_peticion.key,mi_peticion.value_1,mi_peticion.N_value2,mi_peticion.V_value2,mi_peticion.value3);
                break;

            case 2:

                // Caso get_value
                respuesta_a_enviar.resultado_operacion = get_value(mi_peticion.key,mi_peticion.value_1,&mi_peticion.N_value2,mi_peticion.V_value2,&mi_peticion.value3);
                

                break;

            case 3:

                // Caso modify_value
                respuesta_a_enviar.resultado_operacion = modify_value(mi_peticion.key,mi_peticion.value_1,mi_peticion.N_value2,mi_peticion.V_value2,mi_peticion.value3);
                break;

            case 4:

                // Caso delete_key 
                respuesta_a_enviar.resultado_operacion = delete_key(mi_peticion.key);
                break;
            
            case 5:

                // Caso exist
                respuesta_a_enviar.resultado_operacion = exist(mi_peticion.key);
                break;
            default:
                perror("El código de operación específicado no existe");
                respuesta_a_enviar.resultado_operacion = -2;
                break;
        }

        // Una vez preparada la respuesta, la envíamos
        char cod_ejecucion[3] = {'\0'};
        sprintf(cod_ejecucion,"%d",respuesta_a_enviar.resultado_operacion);

        sendMessage(sd,cod_ejecucion,strlen(cod_ejecucion) + 1);

        

        if(mi_peticion.codigo_operacion == 2 && respuesta_a_enviar.resultado_operacion == 0){
           // Enviamos los datos obtenidos solamente si estamos lidiando con el get
            enviar_datos_get(sd,mi_peticion);
        }
        
        // Cerramos el socket al acabar con el cliente actual
        close(sd);
    }
    
    // Finaliza la ejecución del thread
    pthread_exit(0);
   
}

int main(int argc, char *argv[]){

    if(argc !=2){
        perror("Necesitas especificar el número de puerto\n");
        exit(1);
    }

    struct peticion pet;
    pthread_attr_t thread_config;
    pthread_t thread_pool[NUM_THREADS];
    int pos = 0;

    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(sd < 0){
        // El socket no se creo de forma exitosa
        perror("Error in socket");
        exit(1);
    }
    int val = 1;

    int err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

    if( err < 0){
        // La configuración se asignó de forma errónea
        perror("Error en el establecimiento de la configuración");
        exit(1);
    }

    int port_number = atoi(argv[1]);

    if(port_number == 0){
        // Error al hacer el atoi
        perror("No se pudo obtener el puerto del servidor");
        exit(1);
    }

    struct sockaddr_in server_addr;

    bzero((char*)&server_addr, sizeof(server_addr));

    // Establecemos que usamos IPv4
    server_addr.sin_family = AF_INET;

    // Le asigna cualquier interfaz de red de nuestro dispositivo
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Transformamos el puerto a formato de red
    server_addr.sin_port = htons(port_number);

    // Asignamos al socket la configuración deseada
    err = bind(sd, (const struct sockaddr *) &server_addr, sizeof(server_addr));

    if(err == -1){
        printf("Error en el bind\n");
        return -1;
    }

    err = listen(sd,SOMAXCONN);

    if(err == -1){
        printf("Error en el listen\n");
        return -1;
    }


    // Hemos decidido hacer los threads detached de forma que liberan
    // sus recursos de forma automática. 
    pthread_attr_init(&thread_config);
    pthread_mutex_init(&mi_mutex,NULL);
    pthread_cond_init(&no_lleno,NULL);
    pthread_cond_init(&no_vacio,NULL);

    // Creamos la pool de threads
    for(int i = 0; i < NUM_THREADS; i++){
        if(pthread_create(&thread_pool[i],&thread_config,tratar_peticion,NULL) != 0){
            perror("Error creando la pool de threads\n");
            return -1;
        }

    }

    while(1){

    int sc = accept(sd, NULL, NULL);

    if(sc == -1){
        printf("Error en el accept\n");
        continue;
    }

    pthread_mutex_lock(&mi_mutex);

        while(n_elementos == NUM_THREADS){
            // No caben más peticiones en la cola por lo que se debe de esperar
            // antes de recibir más peticiones
            pthread_cond_wait(&no_lleno,&mi_mutex);
        }

        // Guardamos la nueva peticion recibida en el buffer para que pueda ser procesado por 
        // la pool de threads
        buffer_socket[pos] = sc;

        // Aumentamos el índice pos para que la siguiente petición no sobreescriba a la recibida
        pos = (pos + 1) % NUM_THREADS;
        n_elementos++;
        pthread_cond_signal(&no_vacio);
        pthread_mutex_unlock(&mi_mutex);
    }

    pthread_mutex_lock(&mfin);
    fin = 1;
    pthread_mutex_unlock(&mfin);

    pthread_mutex_lock(&mi_mutex);
    pthread_cond_broadcast(&no_vacio);
    pthread_mutex_unlock(&mi_mutex);



    // Ahora nos aseguramos que todos los threads de la pool finalicen su ejecución

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(thread_pool[i],NULL);

    }
    pthread_mutex_destroy(&mi_mutex);
    pthread_cond_destroy(&no_lleno);
    pthread_cond_destroy(&no_vacio);
    pthread_mutex_destroy(&mfin);
}

int obtener_params(int sd,struct peticion *pet){

    char buffer[MAX_V_VALUE2_SIZE];

    // Recibimos el código de operación del cliente
    if(readLine(sd,buffer,MAX_V_VALUE2_SIZE) < 0) return -1;

    pet->codigo_operacion = atoi(buffer);

    switch(pet->codigo_operacion){

        case 0:
        // DESTROY no tiene parámetros
        break;

        case 4: // DELETE KEY
        case 5: // EXIST
        case 2: // GET_VALUE
            // Estos tres casos solamente necesitan recibir la key
            // Recibimos la key del cliente
            if(readLine(sd,buffer,MAX_V_VALUE2_SIZE) < 0) return -1;

            strncpy(pet->key, buffer, MAX_SIZE - 1);
            pet->key[MAX_SIZE - 1] = '\0';

            break;

        case 1: // SET_VALUE
        case 3: // MODIFY_VALUE
            // Necesitan todos los parámetros

             // Recibimos la key del cliente

            if(readLine(sd,buffer,MAX_V_VALUE2_SIZE) < 0) return -1;

            strncpy(pet->key, buffer, MAX_SIZE - 1);
            pet->key[MAX_SIZE - 1] = '\0';

            // Ahora recibimos value_1

            // Primero recibimos la key del cliente
            if(readLine(sd,buffer,MAX_V_VALUE2_SIZE) < 0) return -1;

            strncpy(pet->value_1, buffer, MAX_SIZE - 1);
            pet->value_1[MAX_SIZE - 1] = '\0';

            // Continuamos pasando el n_value2
            
            if(readLine(sd,buffer,MAX_V_VALUE2_SIZE) < 0) return -1;

            pet->N_value2 = atoi(buffer);

            // Continuamos obteniendo el vector V_value2

            if(readLine(sd,buffer,MAX_V_VALUE2_SIZE) < 0) return -1;

            // Ahora lo parseamos para ir guardando los valores en el struct


            char *tokens = strtok(buffer,"[],");
            int i = 0;

            while(tokens != NULL && i < pet->N_value2){

                pet->V_value2[i] = strtof(tokens, NULL);

                // Obtenemos el siguiente número del array
                tokens = strtok(NULL, "[],");
                // Aumentamos la posición de i
                i++;
            }

            // Ahora recibimos la información del paquete como un array de 3 elementos

            if(readLine(sd, buffer, MAX_V_VALUE2_SIZE) < 0) return -1;
            
            tokens = strtok(buffer, "[],");
            if(tokens == NULL) return -1;
            pet->value3.x = atoi(tokens);

            tokens = strtok(NULL, "[],");
            if(tokens == NULL) return -1;
            pet->value3.y = atoi(tokens);

            tokens = strtok(NULL, "[],");
            if(tokens == NULL) return -1;
            pet->value3.z = atoi(tokens);
            break;

        default:
            printf("Código de operación no definido %d\n", pet->codigo_operacion);
            return -1;

    }
    return 0; // Todo se parseó correctamente
   
}


void enviar_datos_get(int sd, struct peticion mi_peticion){


    sendMessage(sd, mi_peticion.value_1, strlen(mi_peticion.value_1) + 1);
        
    char buffer[MAX_V_VALUE2_SIZE];
    sprintf(buffer,"%d", mi_peticion.N_value2);

    sendMessage(sd, buffer, strlen(buffer) + 1);

    // Si es 0, mandamos un array vacio(por seguridad aunque no se puede recibir 0)
    if(mi_peticion.N_value2 == 0){
        strcpy(buffer, "[]");
    }else{

        // Ponemos el corchete inicial para ahora ir añadiendo los números
        strcpy(buffer, "[");
    }

    for(int i = 0; i < mi_peticion.N_value2; i++){
        char tmp[20];

        if( i < mi_peticion.N_value2 - 1){
            // Añadimos el nuevo número junto con una coma
            sprintf(tmp, "%f,", mi_peticion.V_value2[i]);

        }else{
            sprintf(tmp, "%f]", mi_peticion.V_value2[i]);
        }
        strcat(buffer,tmp);
    }

    // Una vez qeu ya tengamos el V_value2, lo enviamos

    sendMessage(sd, buffer, strlen(buffer) + 1);

    // Enviamos value3

    strcpy(buffer, "");
    sprintf(buffer,"[%d,%d,%d]", mi_peticion.value3.x, mi_peticion.value3.y, mi_peticion.value3.z);
    sendMessage(sd, buffer, strlen(buffer) + 1);
    
    
}
