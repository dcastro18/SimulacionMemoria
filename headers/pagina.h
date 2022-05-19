#ifndef SHAREDMEMORY_PAGINA_H
#define SHAREDMEMORY_PAGINA_H

#include "imports.h"

typedef struct Pagina{

    int pag_logica;
    int estado;
    pthread_t thread_id;

} Pagina;


void agregar_pagina(int * n_paginas, int * n_paginas_disp, Pagina * memoria, int pag_logica);

#endif //SHAREDMEMORY_PAGINA_H
