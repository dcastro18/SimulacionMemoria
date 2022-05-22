#ifndef PROCESO_PAGINACION_H
#define PROCESO_PAGINACION_H

#include "proceso.h"

typedef struct Proceso_Paginacion
{
	long int pid;
    Proceso proceso;
    int cant_pags;
} Proceso_Paginacion;


#endif // PROCESO_H