#ifndef SHAREDMEMORY_PRODUCTOR_H
#define SHAREDMEMORY_PRODUCTOR_H

#include "imports.h"



void producir(char *tipoAlgoritmo);
void prod_paginas(int size, int shm_id);
void prod_segmentos(int size, int shm_id);
void crear_hilos_paginas();
void crear_hilos_segmentos();
void registrar_proc(long estado, long thread_id);
void liberar_proc(long estado, long thread_id);
void * reservar_segmentos(void * argv);
void * reservar_paginas(void * n_paginas);
void * ver_memoria_paginada(int n_paginas, void * memoria);
void * ver_memoria_segmentada(int n_paginas, void * memoria_ref);


#endif //SHAREDMEMORY_PRODUCTOR_H
