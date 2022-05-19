#include "../headers/imports.h"


//----------------------------------------------------------
void espiar_mem()
{
    int shm_id = read_int("../data/shm_id.txt");
    sem_t * sem = (sem_t *) solicitar_sem(SEM_NAME);
    void * shm_addr = asociar_mem(shm_id);
    Pagina * memoria = (Pagina *) (shm_addr + OFFSET);
    int * tipo = (int *) (shm_addr + (sizeof(int) * 2));

    bloquear_sem(sem);
    printf("\n---------------   Estado de la memoria   ---------------\n\n");

    if (* tipo == PAGINACION)
        ver_memoria_paginada(*(int *)shm_addr, (void *) memoria);
    else if (* tipo == SEGMENTACION)
        ver_memoria_segmentada(*(int *)shm_addr, (void *) memoria);
    else
        printf("LA MEMORIA NO HA SIDO INICIALIZADA!");

    printf("\n---------------   FIN   ---------------\n");
    desbloquear_sem(sem);
}


//----------------------------------------------------------
void espiar_proc(){

    int shm_id = read_int("../data/shm_id_proc.txt");
    void * shm_addr = asociar_mem(shm_id);
    sem_t * sem = (sem_t *) solicitar_sem(SEM_NAME_PROC);

    bloquear_sem(sem);

    long * array = (long * ) shm_addr;

    printf("\n---------------   Estado de Procesos   ---------------\n\n");

    //----- Proceso Activo ----
    printf("--------> Proceso utilizando memoria compartida <--------\n\t%ld", array[0]);

    //----- Procesos en Espera(Sleep) ----
    printf("\n\n------------> Procesos asignados en memoria <------------");

    array = (long * ) (shm_addr + ESPERA);
    for (int j = 0; (j < 500)  && (array[j] != 0); ++j) {
        if(array[j] != STOP){
            printf("\n\t%ld",array[j]);
        }
    }


    //----- Procesos Bloquedaos ----
    printf("\n\n-------> Procesos bloqueados esperando semáforo <-------");

    array = (long * ) (shm_addr + BLOQUEADO);
    for (int j = 0; (j < 500)  && (array[j] != 0); ++j) {
        if(array[j] != STOP){
            printf("\n\t%ld",array[j]);
        }
    }

    //----- Procesos Muertos(No encontraron espacio) ----
    printf("\n\n------------------> Procesos muertos <------------------");

    array = (long * ) (shm_addr + MUERTO);
    for (int j = 0; (j < 500)  && (array[j] != 0); ++j) {
        if(array[j] != STOP){
            printf("\n\t%ld",array[j]);
        }
    }

    //----- Procesos Finalizados ----
    printf("\n\n----------------> Procesos Finalizados <----------------\n");

    array = (long * ) (shm_addr + FINALIZADO);
    for (int j = 0; (j < 500)  && (array[j] != 0); ++j) {
        if(array[j] != STOP){
            printf("\n\t%ld",array[j]);
        }
    }

    printf("\n\n---------------   FIN   ---------------\n\n");
    desbloquear_sem(sem);
}