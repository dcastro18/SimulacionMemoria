#include "../inicializador.h"


void inicializar(int size)
{
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
    save_int(shm_id, "../data/shm_id.txt");
    shm_addr = asociar_mem(shm_id);

    bloquear_sem(sem);

    // Iniciar arreglo que va a contener estructura de la memoria
    n_celdas = (int *) shm_addr;
    *n_celdas = 0;
    n_celdas_disp = (int *) shm_addr + sizeof(int);
    *n_celdas_disp = 0;
    memoria = (Pagina *) (shm_addr + OFFSET);
    save_int(size, "../data/size.txt");

    ver_memoria_paginada(*n_celdas, (void *) memoria);

    desbloquear_sem(sem);
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