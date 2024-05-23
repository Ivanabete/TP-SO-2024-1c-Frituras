#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <utils/server.h>
#include <pthread.h>
#include <semaphore.h>
#include "conexiones.h"
#include "estructuras.h"
#include "memoriaCPU.h"
#include "memoriaKernel.h"

t_list* lista_de_procesos_con_ins = malloc(sizeof(t_list));

int socket_kernel;
int socket_cpu;
int socket_serv_cpu;
int socket_serv_kernel;

void* escuchar_kernel();
void* escuchar_cpu();