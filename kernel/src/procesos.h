#ifndef PROC_H
#define PROC_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
#include <commons/string.h>
//#include <planificacion.h> // hay que cambiar esto para que planificacion y procesos no se incluyan mutuamente

typedef enum {
    NEW,
    READY,
    BLOCKED,
    EXEC,
    EXIT
}estado_proceso;

typedef struct {
    int PID;
    uint32_t programCounter;
    int quantum;
    int estado;
    // Falta registrosCPU
}PCB;

typedef enum {
    INST_EXIT,
    INST_WAIT,
    INST_SIGNAL,
    SOLICITUD_IO
}motivo_dispatch;

typedef struct {
    PCB* proceso;
    motivo_dispatch motivo;
}t_dispatch; //despues hay que moverlo a utils para que la CPU tambien acceda a la estructura esta (el enum lo mismo)

extern t_log* logger; // no estoy seguro de que esto se vaya a quedar aca

extern sem_t procesosEnNew;
extern sem_t procesosEnExit;
extern sem_t gradoMultiprogramacion;
extern pthread_mutex_t mutexExit;
extern pthread_mutex_t mutexListaProcesos;
extern pthread_mutex_t mutexNew;
extern pthread_mutex_t mutexReady;
extern pthread_mutex_t mutexBlocked;

extern t_queue* colaNew;
extern t_queue* colaReady;
extern t_queue* colaBlocked;
extern t_queue* colaExit;
extern t_list* listadoProcesos;

extern int siguientePID;

// Crea un nuevo PCB y lo envia a la cola NEW
void iniciarProceso(char*);

// Envia a EXIT el proceso con el PID enviado por parametro
void finalizarProceso(int);

// Busca un PCB en base a un PID
PCB* hallarPCB(int);

// Devuelve la cola correspondiente al estado enviado
t_queue* enumEstadoACola(int);

// Saca el PCB enviado por parametro de la cola enviada por parametro.
void sacarProceso(t_queue*, PCB*);

#endif /* PROC_H */