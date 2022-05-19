#include "../headers/segmento.h"


void agregar_segmento(int * n_celdas, Segmento * memoria, int * n_celdas_disp)
{
    memoria[*n_celdas].n_segmento = *n_celdas;
    memoria[*n_celdas].reg_base = *n_celdas;
    memoria[*n_celdas].tamanho = 1;
    memoria[*n_celdas].estado = DISPONIBLE;
    memoria[*n_celdas].thread_id = NULL;
    (*n_celdas)++;
    (*n_celdas_disp)++;
}