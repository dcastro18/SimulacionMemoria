#include "../headers/productor.h"


/*--------------------------------------------------------------------------------------------------------------------*/

void producir(char *tipoAlgoritmo)
{
    int size = read_int("../data/size.txt");
    int shm_id = read_int("../data/shm_id.txt");
    pid_t proc_id = getpid();

    // Guardar ID del proceso
    save_int(proc_id, "../data/prod_id.txt");

    // Segmentacion
    if(strcmp(tipoAlgoritmo,"-s")==0)
    {
        prod_segmentos(size, shm_id);
        crear_hilos_segmentos();
    }

    // Paginacion
    else if(strcmp(tipoAlgoritmo,"-p")==0)
    {
        prod_paginas(size, shm_id);
        crear_hilos_paginas();
    }

    else{
        printf("El tipo de algoritmo ingresado es desconocido, existen:\n\t-s segmentación\n\t-p paginación\n");
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void crear_hilos_paginas()
{
    int tiempo = random_number(30, 60);
    int n_paginas;
    pthread_t thread;

    while(true){
        n_paginas = random_number(1,10);
        pthread_create(&thread, 0, reservar_paginas, &n_paginas);

        sleep((unsigned int)tiempo);

        if (n_paginas < -1) break;
    }
}


/*--------------------------------------------------------------------------------------------------------------------*/

void crear_hilos_segmentos()
{
    pthread_t thread;

    while(true){
        if(pthread_create(&thread, 0, reservar_segmentos, NULL) != 0){
            break;
        }

        sleep((unsigned int) 5);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void registrar_proc(long estado, long thread_id)
{
    int shm_id = read_int("../data/shm_id_proc.txt");
    void * shm_addr = asociar_mem(shm_id);
    sem_t * sem = (sem_t *) solicitar_sem(SEM_NAME_PROC);

    bloquear_sem(sem);

    long * array = (long * ) (shm_addr + estado);

    if(estado == ACTIVO)
        array[0] = thread_id;

    else{
        for (int i = 0; i < 500 ; ++i) {
            if((array[i] == 0) || (array[i] == STOP)){
                array[i] = thread_id;
                break;
            }
        }
    }

    desbloquear_sem(sem);

}


/*--------------------------------------------------------------------------------------------------------------------*/

void liberar_proc(long estado, long thread_id)
{
    int shm_id = read_int("../data/shm_id_proc.txt");
    void * shm_addr = asociar_mem(shm_id);
    sem_t * sem = (sem_t *) solicitar_sem(SEM_NAME_PROC);

    bloquear_sem(sem);

    long * array = (long * ) (shm_addr + estado);

    if(estado == ACTIVO)
        array[0] = STOP;

    else{
        for (int i = 0; i < 500 ; ++i) {
            if(array[i] == thread_id){
                array[i] = STOP;
                break;
            }
        }
    }

    desbloquear_sem(sem);
}



/*--------------------------------------------------------------------------------------------------------------------*/

void * reservar_segmentos(void * argv){

    int n_segmentos = random_number(1,5);

    int n_celdas_segmento = random_number(1,3);

    sem_t * sem = (sem_t *) solicitar_sem(SEM_NAME);
    pthread_t thread_id = pthread_self();
    pthread_detach(thread_id);

    void * shm_addr;
    int shm_id = read_int("../data/shm_id.txt");
    int tiempo = random_number(20,60);

    //Variables para registrar en bitacora
    char * time;
    char buf[256];

    //Registrarse como proceso bloqueado
    registrar_proc(BLOQUEADO, (long) thread_id);

    //Pide el semaforo
    bloquear_sem(sem);

    //Registrarse como proceso desbloqueado
    liberar_proc(BLOQUEADO,(long) thread_id);

    registrar_proc(ACTIVO, (long) thread_id);


    shm_addr = asociar_mem(shm_id);

    int * n_celdas = (int *) shm_addr;
    int * n_celdas_disp = (int *) (shm_addr + sizeof(int));
    Segmento * memoria = (Segmento *) (shm_addr + OFFSET);

    //Arreglo de segmentos encontrados
    int array_segmentos [5] = {};
    int max_celdas = n_segmentos * n_celdas_segmento;

    if (*n_celdas_disp >= max_celdas)
    {
        int n_seg_encontrados = 0;
        for (int j = 0; (j < *n_celdas) && (n_seg_encontrados < n_segmentos);) {
            int n_celdas_libres = 0;
            for (int i = j; (n_celdas_libres < n_celdas_segmento) && (i < *n_celdas); ++i) {

                if(memoria[i].estado == DISPONIBLE)
                    n_celdas_libres++;
                else
                    break;
            }

            if(n_celdas_libres == n_celdas_segmento) {
                array_segmentos[n_seg_encontrados] = j;
                j = j + n_celdas_segmento;
                n_seg_encontrados++;
            }
            else
                j++;
        }

        if(n_seg_encontrados == n_segmentos){//Encontro espacio adecuado
            time = get_time();
            sprintf(buf, "Thread %ld. --> Asignando %d segmentos de memoria.  --> %s", (long)thread_id, n_segmentos , time);
            registrar_accion("../data/bitacora_asignados.txt", buf);

            *n_celdas_disp = *n_celdas_disp - max_celdas;
            for (int i = 0; i < n_segmentos; ++i) {
                int index = array_segmentos[i];
                for (int j = 0; j < n_celdas_segmento; ++j) {
                    memoria[index + j].estado = OCUPADO;
                    memoria[index + j].n_segmento = i + 1;
                    memoria[index + j].reg_base = index;
                    memoria[index + j].tamanho = n_celdas_segmento;
                    memoria[index + j].thread_id = thread_id;
                }
                sprintf(buf, "\tSegmento %d asignado",i+1);
                registrar_accion("../data/bitacora_asignados.txt", buf);
            }
            time = get_time();
            sprintf(buf, "\tAsignación finalizada con éxito. --> %s\tEsperando %d segundos para liberar memoria\n", time, tiempo);
            registrar_accion("../data/bitacora_asignados.txt", buf);

            desbloquear_sem(sem);
            liberar_proc(ACTIVO, (long) thread_id);

            registrar_proc(ESPERA, (long) thread_id);
            ver_memoria_segmentada(*(int *)shm_addr, (void *) memoria);
            sleep((unsigned int) tiempo);
            liberar_proc(ESPERA, (long) thread_id);


            registrar_proc(BLOQUEADO, (long) thread_id);

            bloquear_sem(sem);

            liberar_proc(BLOQUEADO, (long) thread_id);

            registrar_proc(ACTIVO, (long) thread_id);

            // Eliminar memoria de segmentos
            time = get_time();
            sprintf(buf, "Thread %ld. --> Desasignando %d segmentos de memoria.  --> %s", (long)thread_id, n_segmentos , time);
            registrar_accion("../data/bitacora_asignados.txt", buf);

            for (int i = 0; (i < * n_celdas) && (max_celdas > 0); ++i)
            {
                if ((memoria[i].estado == OCUPADO) && ((long) memoria[i].thread_id == (long) thread_id))
                {
                    memoria[i].estado = DISPONIBLE;
                    memoria[i].thread_id = NULL;
                    memoria[i].n_segmento = i;
                    memoria[i].reg_base = i;
                    memoria[i].tamanho = 1;

                    max_celdas--;

                    sprintf(buf, "\tSegmento %d desasignado",i);
                    registrar_accion("../data/bitacora_asignados.txt", buf);
                }
            }

            *n_celdas_disp = *n_celdas_disp + (n_segmentos * n_celdas_segmento);

            time = get_time();
            sprintf(buf, "\tDesasignación finalizada con éxito. --> %s", time);
            registrar_accion("../data/bitacora_asignados.txt", buf);


        }
        else{//No encontro espacio adecuado
            time = get_time();
            sprintf(buf, "El thread %ld no encontró espacios suficientes. \n\tSolicitaba %d segmentos. \n\tHora: %s", (long)thread_id, n_segmentos, time);
            registrar_accion("../data/bitacora_fallidos.txt", buf);

            registrar_proc(MUERTO, (long) thread_id);
        }
    }

    else
    {
        time = get_time();
        sprintf(buf, "El thread %ld no encontró espacios suficientes. \n\tSolicitaba %d segmentos. \n\tHora: %s", (long)thread_id, n_segmentos, time);
        registrar_accion("../data/bitacora_fallidos.txt", buf);

        registrar_proc(MUERTO, (long) thread_id);
    }

    ver_memoria_segmentada(*(int *)shm_addr, (void *) memoria);

    desbloquear_sem(sem);
    liberar_proc(ACTIVO, (long) thread_id);
    registrar_proc(FINALIZADO, (long) thread_id);

}

/*--------------------------------------------------------------------------------------------------------------------*/

void * reservar_paginas(void * ref_n_paginas)
{
    int n_paginas = (*(int *)ref_n_paginas);
    int n_paginas_aux = n_paginas;
    sem_t * sem = (sem_t *) solicitar_sem(SEM_NAME);
    pthread_t thread_id = pthread_self();
    pthread_detach(thread_id);
    void * shm_addr;
    int shm_id = read_int("../data/shm_id.txt");
    int tiempo = random_number(20,60);

    //Variables para registrar en bitacora
    char * time;
    char buf[256];

    registrar_proc(BLOQUEADO, (long) thread_id);

    bloquear_sem(sem);

    liberar_proc(BLOQUEADO, (long) thread_id);

    registrar_proc(ACTIVO, (long) thread_id);

    shm_addr = asociar_mem(shm_id);

    int * n_celdas = (int *) shm_addr;
    int * n_celdas_disp = (int *) (shm_addr + sizeof(int));
    Pagina * memoria = (Pagina *) (shm_addr + OFFSET);

    if (*n_celdas_disp > n_paginas)
    {
        time = get_time();
        sprintf(buf, "Thread %ld. --> Asignando %d páginas de memoria.  --> %s", (long)thread_id, n_paginas , time);
        registrar_accion("../data/bitacora_asignados.txt", buf);

        *n_celdas_disp = *n_celdas_disp - n_paginas;
        for (int i = 0; (i < * n_celdas) && (n_paginas > 0); ++i)
        {
            if (memoria[i].estado == DISPONIBLE)
            {
                memoria[i].estado = OCUPADO;
                memoria[i].thread_id = thread_id;
                n_paginas--;

                sprintf(buf, "\tPágina %d asignada",i);
                registrar_accion("../data/bitacora_asignados.txt", buf);
            }
        }

        time = get_time();
        sprintf(buf, "\tAsignación finalizada con éxito. --> %s\tEsperando %d segundos para liberar memoria\n", time, tiempo);
        registrar_accion("../data/bitacora_asignados.txt", buf);

        desbloquear_sem(sem);
        liberar_proc(ACTIVO, (long) thread_id);

        registrar_proc(ESPERA, (long) thread_id);
        ver_memoria_paginada(*(int *)shm_addr, (void *) memoria);
        sleep((unsigned int) tiempo);
        liberar_proc(ESPERA, (long) thread_id);


        registrar_proc(BLOQUEADO, (long) thread_id);

        bloquear_sem(sem);

        liberar_proc(BLOQUEADO, (long) thread_id);
        registrar_proc(ACTIVO, (long) thread_id);

        // Eliminar memoria
        time = get_time();
        sprintf(buf, "Thread %ld. --> Desasignando %d páginas de memoria.  --> %s", (long)thread_id, n_paginas_aux , time);
        registrar_accion("../data/bitacora_asignados.txt", buf);

        for (int i = 0; (i < * n_celdas) && (n_paginas < n_paginas_aux); ++i)
        {
            if ((memoria[i].estado == OCUPADO) && ((long) memoria[i].thread_id == (long) thread_id))
            {
                memoria[i].estado = DISPONIBLE;
                memoria[i].thread_id = NULL;
                n_paginas++;
                sprintf(buf, "\tPágina %d desasignada",i);
                registrar_accion("../data/bitacora_asignados.txt", buf);
            }
        }
        *n_celdas_disp = *n_celdas_disp + n_paginas;

        time = get_time();
        sprintf(buf, "\tDesasignación finalizada con éxito. --> %s", time);
        registrar_accion("../data/bitacora_asignados.txt", buf);
    }
    else
    {
        time = get_time();
        sprintf(buf, "El thread %ld no encontró espacios suficientes. Solicitaba %d páginas. Hora: %s", (long)thread_id, n_paginas_aux, time);
        registrar_accion("../data/bitacora_fallidos.txt", buf);

        registrar_proc(MUERTO, (long) thread_id);
    }
    ver_memoria_paginada(*(int *)shm_addr, (void *) memoria);
    desbloquear_sem(sem);
    liberar_proc(ACTIVO, (long) thread_id);

    registrar_proc(FINALIZADO, (long) thread_id);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void prod_paginas(int size, int shm_id)
{
    sem_t * sem = (sem_t *) solicitar_sem(SEM_NAME);
    void * shm_addr = asociar_mem(shm_id);

    bloquear_sem(sem);

    int * n_celdas = (int *) shm_addr;
    int * n_celdas_disp = (int *) (shm_addr + sizeof(int));
    int * tipo = (int *) (shm_addr + (sizeof(int) * 2));
    Pagina * memoria = (Pagina *) (shm_addr + OFFSET);
    * n_celdas = 0;
    * n_celdas_disp = 0;
    * tipo = PAGINACION;
    for (int i = 0; (i < size) && (*n_celdas < size); i++)
    {
        agregar_pagina(n_celdas, n_celdas_disp, memoria, *n_celdas);
    }

    ver_memoria_paginada(*(int *)shm_addr, (void *) memoria);

    desbloquear_sem(sem);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void prod_segmentos(int size, int shm_id)
{
    sem_t * sem = (sem_t *) solicitar_sem(SEM_NAME);

    void * shm_addr = asociar_mem(shm_id);

    bloquear_sem(sem);

    int * n_celdas = (int *) shm_addr;
    int * tipo = (int *) (shm_addr + (sizeof(int) * 2));
    int * n_celdas_disp = (int *) (shm_addr + sizeof(int));
    Segmento * memoria = (Segmento *) (shm_addr + OFFSET);
    * n_celdas = 0;
    * n_celdas_disp = 0;
    * tipo = SEGMENTACION;


    for (int i = 0; (i < size) && (*n_celdas < size); i++)
    {
        agregar_segmento(n_celdas, memoria, n_celdas_disp);
    }

    ver_memoria_segmentada(*(int *)shm_addr, (void *) memoria);

    desbloquear_sem(sem);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void * ver_memoria_paginada(int n_paginas, void * memoria_ref)
{
    int shm_id = read_int("../data/shm_id.txt");
    void * shm_addr = asociar_mem(shm_id);
    int * pag_disp = (int *) (shm_addr + sizeof(int));

    Pagina * memoria = (Pagina *) memoria_ref;
    printf("\nPaginas disponibles: %d\n", *pag_disp);
    for (int i = 0; i < n_paginas; i++) {
        printf("P.Logica: %d | Estado:%d | Proc.ID:%ld\n",
               memoria[i].pag_logica, memoria[i].estado, (long) memoria[i].thread_id);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void * ver_memoria_segmentada(int n_paginas, void * memoria_ref)
{
    int shm_id = read_int("../data/shm_id.txt");
    void * shm_addr = asociar_mem(shm_id);
    int * celdas_disp = (int *) (shm_addr + sizeof(int));
    Segmento * memoria = (Segmento *) memoria_ref;
    printf("\nPaginas disponibles: %d\n", *celdas_disp);

    for (int i = 0; i < n_paginas;i++) {
        printf("N-Segmento: %d | R.Base: %d | Tamanho: %d | Estado:%d | Proc.ID:%ld\n",
               memoria[i].n_segmento, memoria[i].reg_base, memoria[i].tamanho, memoria[i].estado, (long) memoria[i].thread_id);

    }
}