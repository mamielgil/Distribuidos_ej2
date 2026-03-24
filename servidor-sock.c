#include "claves.h"
#include <fcntl.h>        
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

// Consideramos el defecto de Linux
#define NUM_MENSAJES 10
#define NUM_THREADS 50
# define MAX_SIZE 256


/* PREGUNTAR AL PROFE SI AL USAR  THREADS BAJO DEMANDA SE PUEDE PONER LISTEN
AL NUMERO DE THREADS PARA QUE SI LLEGAN MAS CLIENTES SE DESCARTEN Y TENGAN QUE VOLVER
A PEDIR UN SITIO EN EL SERVER

SI QUEREMOS USAR FUNCIONES LINES.C DADAS EN CLASE COMO NO PODEMOS INCLUIR EL HEADER
LAS DECLARAMOS DE FORMA INDIVIDUAL EN CLIENTE Y SERVIDOR


COMO DETERMINAR EL TAMAÑO DEL BUFFER A MANDAR ENTRE SERVIDOR Y PROXY CON LOS PARAMETROS
ESCOGEMOS UN TAMAÑO GRANDE ARBITRARIO? O HAY OTRA FORMA

RECOMIENDA USAR READLINE
*/


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
int n_elementos;
int pos_servicio = 0;
pthread_mutex_t mi_mutex;
pthread_cond_t no_lleno;
pthread_cond_t no_vacio;
pthread_mutex_t mfin;
int fin = 0;


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
        obtener_params(sd, &mi_peticion);


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

        if(cola_cliente == -1){
            perror("No se pudo abrir la cola del cliente. Por lo tanto, la respuesta no pudo ser procesada");

        } else{
            // Enviamos la respuesta al cliente y cerramos su cola
            mq_send(cola_cliente,(const char *) &respuesta_a_enviar,sizeof(struct respuesta),0);
            mq_close(cola_cliente);

        }


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

    err = listen(sd, NUM_THREADS);

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

void obtener_params(int sd,struct peticion *pet){

    // char buffer[] ={key: hola\n 
                    //N_value2: 56\n}
    
    // readLine(sd,)





}


// Implementación de lines.c dada en clase

int sendMessage(int socket, char * buffer, int len)
{
	int r;
	int l = len;
		

	do {	
		r = write(socket, buffer, l);
		l = l -r;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fail */
	else
		return(0);	/* full length has been sent */
}



ssize_t readLine(int fd, void *buffer, size_t n)
{
	ssize_t numRead;  /* num of bytes fetched by last read() */
	size_t totRead;	  /* total bytes read so far */
	char *buf;
	char ch;


	if (n <= 0 || buffer == NULL) { 
		errno = EINVAL;
		return -1; 
	}

	buf = buffer;
	totRead = 0;
	
	for (;;) {
        	numRead = read(fd, &ch, 1);	/* read a byte */

        	if (numRead == -1) {	
            		if (errno == EINTR)	/* interrupted -> restart read() */
                		continue;
            	else
			return -1;		/* some other error */
        	} else if (numRead == 0) {	/* EOF */
            		if (totRead == 0)	/* no byres read; return 0 */
                		return 0;
			else
                		break;
        	} else {			/* numRead must be 1 if we get here*/
            		if (ch == '\n')
                		break;
            		if (ch == '\0')
                		break;
            		if (totRead < n - 1) {		/* discard > (n-1) bytes */
				totRead++;
				*buf++ = ch; 
			}
		} 
	}
	
	*buf = '\0';
    	return totRead;
}
