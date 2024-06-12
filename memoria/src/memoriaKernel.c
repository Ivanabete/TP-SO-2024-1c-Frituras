#include <utils/serializacion.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include "estructuras.h"
#include "memoriaDeInstrucciones.h"
#include "memoriaCPU.h"
#include "main.h"

t_list* lista_de_procesos = NULL;
t_log* log_tabla_pags; 
pthread_mutex_t mutex_lista_procesos = PTHREAD_MUTEX_INITIALIZER;

t_proceso_memoria* creacion_proceso(int socket_kernel, int tiempo_retardo) {
    op_code cod_op = recibir_operacion(socket_kernel);
    sleep(tiempo_retardo/1000);
    log_tabla_pags = log_create("Tabla_de_Pags_Memoria", "Memoria", false, LOG_LEVEL_INFO);
    if(cod_op == CREACION_PROCESO){
        t_proceso_memoria* proceso = malloc(sizeof(proceso));
        t_buffer* buffer = recibir_buffer(socket_kernel);
        int pid_proceso= buffer_read_int(buffer);
        proceso->pid=pid_proceso;
        char* path_proceso = buffer_read_string(buffer);
        proceso->path= path_proceso;
        proceso->tabla_del_proceso = list_create(); // info arranca como lista vacía ya q arranca todo vacío
        proceso->tamanio_proceso = 0;
        proceso->instrucciones = list_create(); // arranca vacía pq todavia no le llegan las instrucciones
        proceso->pc = 0; // esto es para ya tenerlo seteado, pero luego se va a cambiar
        liberar_buffer(buffer);
        pthread_mutex_lock (&mutex_lista_procesos);
        list_add(lista_de_procesos,proceso); // guardo en la lista de los procesos el proceso!
        pthread_mutex_unlock(&mutex_lista_procesos);
        log_info(log_tabla_pags, "Creacion Tabla de Paginas - PID: %d- Tamaño: %d", proceso->pid, list_size(proceso->tabla_del_proceso));
        return proceso;
    }
    return NULL;
}

void abrir_archivo_path(int socket_kernel, int tiempo_retardo){
    t_proceso_memoria* proceso = creacion_proceso(socket_kernel, tiempo_retardo);
    char* path = (proceso->path);
    if (path == NULL){
        exit(1);
    }
    FILE *file = fopen(path,"r");
    if (file == NULL){
            fprintf(stderr, "Archivo vacio");
            free(path);
        }
    char* linea = NULL;
    size_t tamanio = 0;
    ssize_t leidos;
   
    while ((leidos = getline(&linea, &tamanio,file)) != -1){
        if(linea[leidos - 1] == '\n'){
            linea[leidos - 1] = '\0';
        }

        char *linea_copia = strdup(linea);
        if (linea_copia == NULL){
            perror("Error al copiar linea");
            free(linea);
            fclose(file);
            return;
        }
        list_add(proceso->instrucciones, linea_copia);
    }
    t_paquete* paquete = crear_paquete(PAQUETE); // NO BORRAR! esto es para que conecten bien los módulos 
    enviar_paquete(paquete, socket_kernel);

    free(linea);
    fclose(file);
}

void eliminar_proceso(int pid_proceso){
    t_proceso_memoria* proceso = hallar_proceso(pid_proceso);
    list_remove(lista_de_procesos, (proceso->pid));
    free(proceso);
}
void frames_libres_por_fin_proceso(t_proceso_memoria* proceso_a_eliminar){
    int tamanio_tabla = list_size(proceso_a_eliminar->tabla_del_proceso);

    for(int i = 0; i < tamanio_tabla; i++){
        informacion_de_tabla* entrada = list_get(proceso_a_eliminar->tabla_del_proceso, i);
        pthread_mutex_lock(&mutex_bitarray_marcos_libres);

        if(entrada->validez){
            bitarray_clean_bit(mapa_de_marcos,entrada->marco);
        }
        pthread_mutex_unlock(&mutex_bitarray_marcos_libres);
    }
    }

void finalizar_proceso(int socket_kernel, int tiempo_retardo){
    op_code cod_op = recibir_operacion(socket_kernel);
    sleep(tiempo_retardo/1000);
    log_tabla_pags = log_create("Tabla_de_Pags_Memoria", "Memoria", false, LOG_LEVEL_INFO);
    if(cod_op == FIN_PROCESO){
        t_buffer* buffer = recibir_buffer(socket_kernel);
        int pid_proceso= buffer_read_int(buffer);
        t_proceso_memoria* proceso_a_finalizar = hallar_proceso(pid_proceso);
        frames_libres_por_fin_proceso(proceso_a_finalizar);
        log_info(log_tabla_pags, "Destruccion Tabla Paginas - PID: %d- Tamaño: %d", pid_proceso, list_size(proceso_a_finalizar->tabla_del_proceso));
        eliminar_proceso_de_lista_de_procesos(pid_proceso);
        liberar_buffer(buffer);
        pthread_mutex_destroy(mutex_lista_procesos);
    }
}
