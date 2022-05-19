#ifndef SHAREDMEMORY_BIBLIOTECAS_H
#define SHAREDMEMORY_BIBLIOTECAS_H


// Generales
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

// Otros headers
#include "inicializador.h"
#include "productor.h"
#include "espia.h"
#include "finalizador.h"
#include "herramientas.h"
#include "pagina.h"
#include "segmento.h"
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

// Memoria Compartida
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Semaforos
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>


// Variables globales
#define SEGMENTSIZE     20480
#define SEGMENTPERM     0666
#define SEM_NAME        "/Shared_memory"
#define SEM_NAME_PROC   "/Proc_status"
#define OCUPADO         0
#define DISPONIBLE      1
#define OFFSET          (sizeof(int) * 3)
#define PAGINACION      0
#define SEGMENTACION    1
#define LONG_SBLOCK     (sizeof(long) * 500)
#define STOP            (-99)

#define ACTIVO          0
#define BLOQUEADO       (sizeof(long))
#define MUERTO          BLOQUEADO + LONG_SBLOCK
#define FINALIZADO      MUERTO + LONG_SBLOCK
#define ESPERA          FINALIZADO + LONG_SBLOCK


#endif //SHAREDMEMORY_BIBLIOTECAS_H
