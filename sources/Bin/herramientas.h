#ifndef SHAREDMEMORY_HERRAMIENTAS_H
#define SHAREDMEMORY_HERRAMIENTAS_H

#include <time.h>
#include "imports.h"

// General
int random_number(int min_num, int max_num);
void registrar_accion(char * file_name, char * registro);
void save_int(int num, char * file_name);
int read_int(char * file_name);
char * get_time();

// Semaforos
void * solicitar_sem(char * nombre_sem);
void bloquear_sem(void * sem_ref);
void desbloquear_sem(void * sem_ref);
void cerrar_sem(void * sem_ref);

// Memoria
int solicitar_mem();
void * asociar_mem(int id);

#endif //SHAREDMEMORY_HERRAMIENTAS_H