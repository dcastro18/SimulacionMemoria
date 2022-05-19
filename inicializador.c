
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>

#include "proceso.h"

// Crea o limmpia el archivo de bitácora
void crearBitacora(){
    FILE *f = fopen ("bitacora.txt", "w");
    if(f == NULL){
        printf("Error al abrir el archivo\n");
        exit(1);
    }
    fclose(f);
}


void inic_proc_mem(){
    int shm_id;
    void * shm_addr;

    // Crear espacio de memoria de procesos
    shm_id = solicitar_mem();
    save_int(shm_id, "../data/shm_id_proc.txt");
    shm_addr = asociar_mem(shm_id);

    for (int i = 0; i < 2001; ++i) {
        long *array_bloquedos = (long *) (shm_addr + (sizeof(long) * i));
        *array_bloquedos = 0;
    }
}

int main()
{
    int size = 0;
    while(size <= 0){
        printf("Ingrese el número de paginas o espacios de memoria: ");
        scanf("%d", &size);

        if(size <= 0){
            printf("Por favor, ingrese un número positivo.\n\n");
        }
    }

    int shm_id, *n_celdas, *n_celdas_disp;
    void * shm_addr;
    sem_t * sem = NULL;
    Pagina * memoria;

    // Inicializar el semaforo
    sem = (sem_t *) solicitar_sem(SEM_NAME);

    //Crear espacio memoria de procesos
    inic_proc_mem();
    //Crear espacio memoria principal
    shm_id = solicitar_mem();
    save_int(shm_id, "../shm_id.txt");
    shm_addr = asociar_mem(shm_id);

    // Bloquea el semaforo
    bloquear_sem(sem);

    // Iniciar arreglo que va a contener estructura de la memoria
    n_celdas = (int *) shm_addr;
    *n_celdas = 0;
    n_celdas_disp = (int *) shm_addr + sizeof(int);
    *n_celdas_disp = 0;
    memoria = (Pagina *) (shm_addr + OFFSET);
    // Guarda el tamanho dado en el txt
    save_int(size, "../size.txt");

    ver_memoria_paginada(*n_celdas, (void *) memoria);

    desbloquear_sem(sem);

    return 0;
}
