#include "../headers/imports.h"

void finalizar()
{
    int shm_id = read_int("../data/shm_id.txt");
    int shm_id_proc = read_int("../data/shm_id_proc.txt");

    /*Eliminacion de los procesos del productor*/
    system("killall prod");
    printf("\nProcesos eliminados\n");

    /*Eliminacion de la memoria compartida simulada*/
    if (shmctl(shm_id, IPC_RMID, 0) != -1)
        printf("\nMemoria compartida liberada\n");

    /*Eliminacion de la memoria compartida para control de procesos*/
    if (shmctl(shm_id_proc, IPC_RMID, 0) != -1)
        printf("\nMemoria compartida de procesos liberada\n");

    /*Eliminacion del semaforo de la memoria simulada*/
    if (sem_unlink(SEM_NAME) == 0)
        printf("\nSemaforo de memoria compartido eliminado\n");

    /*Eliminacion del semaforo de la memoria para control de procesos*/
    if (sem_unlink(SEM_NAME_PROC) == 0)
        printf("\nSemaforo de memoria de procesos eliminado\n");
}