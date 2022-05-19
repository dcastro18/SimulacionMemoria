#ifndef SHAREDMEMORY_SEGMENTO_H
#define SHAREDMEMORY_SEGMENTO_H

#include "imports.h"

typedef struct Segmento{
    int n_segmento;
    int reg_base;
    int tamanho;
    int estado;
    pthread_t thread_id;
} Segmento;


void agregar_segmento(int * n_celdas, Segmento * memoria, int * n_celdas_disp);


#endif //SHAREDMEMORY_SEGMENTO_H
