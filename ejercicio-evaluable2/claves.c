#include "claves.h"
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/file.h>

struct elemento {
    // Definimos el tipo de dato que se va a almacenar en la lista
    char key[256];
    char value1[256];
    int N_value2; // Máximo valor es 32
    float V_value2[32]; // Como máximo tamaño es 32, lo ajustamos a 32 elementos directamente
    struct Paquete value3;


};

int exist(char *key){

    char path[512];
    snprintf(path, sizeof(path), "./clientes/%s", key);
    DIR *dir;
    if ((dir = opendir("./clientes")) == NULL) {
        if (errno == ENOENT) {
            mkdir("./clientes", 0755);
        }
        return 0; // Retornamos 0 en cualquier caso de error al abrir
    }

    closedir(dir);

    // Ahora para comprobar si existe el archivo, vamos a realizar una lectura. Por lo tanto,
    // bloquedamos el archivo para que solamente puedan acceder lectores

    int fd;
    if((fd = open(path, O_RDONLY)) < 0){
        return 0;

    } else {
        // Permitimos solamente lectura
        if(flock(fd,LOCK_SH) == -1){
            close(fd);
            return 0;
        }

        flock(fd,LOCK_UN);
        close(fd);
        return 1;
    }
}


int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){

    // Nos aseguramos de que el directorio exista antes de intentar crear el archivo
    if (mkdir("./clientes", 0755) == -1 && errno != EEXIST) {
        // Solamente damos error si no se pudo crear el directorio o abrirlo por un error distinto a que ya existía
        return -1;
    }

    char path[512];
    snprintf(path, sizeof(path), "./clientes/%s", key);

    int fd;
    if((fd = open(path, O_CREAT | O_WRONLY | O_APPEND | O_EXCL, 0666)) < 0){
        // Error porque la key ya existe o porque no se pudo abrir el archivo
        return -1;
    }
    
    // Para establecer un valor, realizamos un bloqueo exclusivo
    if(flock(fd,LOCK_EX) == -1){
        // No se pudo aplicar el lock
        close(fd);
        return -1;
    }

    // Comprobamos que N_value2 esté dentro del rango permitido
    if(N_value2 > 32 || N_value2 <= 0){
        flock(fd,LOCK_UN);
        close(fd);
        return -1;
    }
    // Creamos el elemento a enviar

    struct elemento elemento_guardar;
    memset(&elemento_guardar,0,sizeof(struct elemento));
    strcpy(elemento_guardar.key, key);
    strcpy(elemento_guardar.value1, value1);
    elemento_guardar.N_value2 = N_value2;
    memcpy(elemento_guardar.V_value2, V_value2, sizeof(float) * N_value2);
    elemento_guardar.value3 = value3;
    
    int bytes_escritos = 0;
    int code_error = 0;


    do{
        // Escribimos hasta asegurar que se haya guardado todo el elemento
        code_error = write(fd, ((char*)&(elemento_guardar) + bytes_escritos), sizeof(struct elemento) - bytes_escritos);
        if(code_error == -1){
            // Error de lectura, finalizamos la ejecución,
            // asegurándonos que se desbloquea el archivo
            flock(fd,LOCK_UN);
            close(fd);
            return -1;
        }else{
            bytes_escritos += code_error;
        }
    }while(bytes_escritos != sizeof(struct elemento));

    // Desbloqueamos el archivo al finalizar
    flock(fd,LOCK_UN);
    close(fd);
    return 0;

}


int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3){
    char path[512];
    snprintf(path, sizeof(path), "./clientes/%s", key);

    if (exist(key) == 0){
        // No existe el archivo
        return -1;
    }

    int fd;
    if((fd = open(path, O_RDONLY)) < 0){
        return -1;
    }

    // Leemos y buscamos la key

    struct elemento elemento_leido;
    int bytes_leidos = 0;

    // Realizamos un bloqueo de lectura

    if(flock(fd,LOCK_SH) == -1){
        close(fd);
        return -1;
    }

    while( (bytes_leidos = read(fd,(void *) &elemento_leido, sizeof(struct elemento))) > 0){
    
        // Comprobamos si se ha leido el struct completo
        if(bytes_leidos == sizeof(struct elemento)){

            if(strcmp(key,elemento_leido.key) == 0){

                // El elemento se encontró pr lo que guardamos sus valores
                strcpy(value1, elemento_leido.value1);
                *N_value2 = elemento_leido.N_value2;
                memcpy(V_value2,elemento_leido.V_value2,sizeof(float) * elemento_leido.N_value2);
                *value3 = elemento_leido.value3;
                flock(fd,LOCK_UN);
                close(fd);
                return 0;
            }
        }

    }
    if(bytes_leidos == -1){
        //Hubo un error durante la lectura
        flock(fd,LOCK_UN);
        close(fd);
        return -1;

    }
    // El elemento no se encontró
    flock(fd,LOCK_UN);
    close(fd);
    return -1;

}

int destroy(void){
    char path[512]; // Buffer para construir la ruta completa
    DIR * direct = opendir("./clientes");

    if(direct == NULL){
        
        if(errno == ENOENT){
            // El directorio no existe por lo que asumimos que está borrado correctamente
            return 0;
        }else{
            // El directorio no se pudo abrir
            return -1;
        }
    }

    struct dirent * entry;
    while ((entry = readdir(direct)) != NULL){
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, sizeof(path), "./clientes/%s", entry->d_name);

        // Abrimos el archivo para poder hacer un lock mientras es borrado
        int fd;
        if((fd = open(path,O_RDONLY,0666)) == -1){
            // El archivo no se pudo abrir correctamente, al no poder vaciar el directorio completo asumimos que hubo un error
            return -1;
        }
        if(flock(fd,LOCK_EX) == -1){
            // No se pudo realizar el lock
            close(fd);
            return -1;
        }
        if (unlink(path) != 0){
            // No se pudo borrar la entrada
            flock(fd,LOCK_UN);
            close(fd);
            closedir(direct);
            return -1;
        }
        flock(fd,LOCK_UN);
        close(fd); 

    }

    // Si hemos salido del loop, se han borrado todos los archivos
    closedir(direct);
	return 0;
}

int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
    char path[512];
    snprintf(path, sizeof(path), "./clientes/%s", key);

    if (exist(key) == 0){
        return -1;
    }
		
    if(N_value2 < 1 || N_value2 > 32){
        // El valor de N_value 2 no es correcto
        return -1;
    }
    int fd;
    if((fd = open(path, O_RDWR)) < 0){
        return -1;
    }

    // Leemos y buscamos la key

    struct elemento elemento_leido;
    int bytes_leidos = 0;

    // Realizamos un lock exclusive

    if(flock(fd,LOCK_EX) == -1){
        close(fd);
        return -1;
    }
    while( (bytes_leidos = read(fd,(void *) &elemento_leido, sizeof(struct elemento))) > 0){
    
        // bytes_leidos / sizeof(struct elemento) corresponde al números de structs a leer
        if(bytes_leidos == sizeof(struct elemento)){

            //Seleccionamos el struct leido
            if(strcmp(key,elemento_leido.key) == 0){

            // El elemento se encontró pr lo que lo modificamos
                strcpy(elemento_leido.value1,value1);
                elemento_leido.N_value2 = N_value2;
                memcpy(elemento_leido.V_value2,V_value2,sizeof(float) * N_value2);
                elemento_leido.value3 = value3;

                // El elemento esta actualizado, movemos el puntero para escribirlo en el archivo
                if (lseek(fd,0, SEEK_SET) == -1){
                // La modificación no se pudo hacer de forma correcta
                    flock(fd,LOCK_UN);
                    close(fd);
                    return -1;

                }

                // Ahora escribimos para sobreescribir el elemento
                int bytes_escritos = 0;
                int code_error = 0;
                do{
                // Escribimos hasta asegurar que se haya guardado todo el elemento
                    code_error = write(fd, ((char*)&(elemento_leido) + bytes_escritos), sizeof(struct elemento) - bytes_escritos);
                    if(code_error == -1){

                        // Error de lectura, finalizamos la ejecución
                        flock(fd,LOCK_UN);
                        close(fd);
                        return -1;
                    }else{
                        bytes_escritos += code_error;
                    }
                }while(bytes_escritos != sizeof(struct elemento));
            // Si hemos llegado hasta aqui significa que se realizo la sobreescritura
            flock(fd,LOCK_UN);
            close(fd);
            return 0;        
            }
        }
    }
    flock(fd,LOCK_UN);
    close(fd);
    return -1;

}


int delete_key(char *key){
    char path[512];
    snprintf(path, sizeof(path), "./clientes/%s", key);

    if (exist(key) == 0){
        // No existe el archivo así que se devuelve -1
        return -1;
    }
    // Ahora abrimos el archivo para hacer el lock
    int fd;
    if((fd = open(path,O_RDONLY,0666)) == -1){
        // No se pudo abrir el archivo
        return -1;
    }
    // Hacemos un lock del archivo
    if(flock(fd,LOCK_EX) == -1){
        // No se pudo hacer el lock
        close(fd);
        return -1;

    }

    if (unlink(path) == -1){
        flock(fd,LOCK_UN);
        close(fd);
        return -1;
    }

    // Cerramos el file descriptor del archivo borrado
    flock(fd,LOCK_UN);
    close(fd);
    return 0;
}
