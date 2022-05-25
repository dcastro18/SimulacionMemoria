#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <stdio.h>

#include "proceso.h"

// Variables globales
Proceso *readyQueue_mem;
int *control_mem;
Proceso *procesos_mem;
sem_t sem_ready, sem_procesos;

/**
    Retorna un numero entero aleatorio entre [min, max]
*/
int getRandom(int min, int max)
{
    srand(time(0));
    int numero = (rand() % (max - min + 1)) + min;
    return numero;
}

/*
    Agrega un proceso a la memoria de procesos
    Retorna la posición donde lo metió
*/
int agregarProceso(Proceso *proceso)
{
    // pide el semáforo
    sem_wait(&sem_procesos);

    int pos = control_mem[2]; 
    procesos_mem[pos].pid = proceso->pid;
    procesos_mem[pos].cant_pags = proceso->cant_pags;
    memcpy(procesos_mem[pos].segmentos, proceso->segmentos, sizeof(procesos_mem[pos].segmentos));
    procesos_mem[pos].tiempo = proceso->tiempo;

    control_mem[2] = control_mem[2] + 1;

    // devuelve el semáforo
    sem_post(&sem_procesos);

    return pos;
}

/*
    Elimina un proceso de la memoria de procesos vivos y reordena
*/
void eliminarProceso(long int idProceso)
{
    // pide el semáforo
    sem_wait(&sem_procesos);

    for (int i = 0; i < control_mem[2]; i++)
    {
        if (procesos_mem[i].pid == idProceso && procesos_mem[i].estado != Muerto)
        {
            procesos_mem[i].estado = Fuera;
        }
    }

    control_mem[4] = control_mem[4] + 1;

    // devuelve el semáforo
    sem_post(&sem_procesos);
}

/*
    Elimina un proceso de la memoria de procesos vivos y reordena
*/
void matarProceso(long int idProceso)
{
    // pide el semáforo
    sem_wait(&sem_procesos);

    for (int i = 0; i < control_mem[2]; i++)
    {
        if (procesos_mem[i].pid == idProceso)
        {
            procesos_mem[i].estado = Muerto;
        }
    }

    control_mem[5] = control_mem[5] + 1;

    // devuelve el semáforo
    sem_post(&sem_procesos);
}

//============================================================
//                  ALGORITMO DE BITÁCORA
//============================================================
void escBitacora(const char *texto)
{
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char output[128];
    strftime(output, 128, "%d/%m/%Y %H:%M:%S", tlocal);

    FILE *f = fopen("bitacora.txt", "a");
    if (f == NULL)
    {
        printf("Error al abrir el archivo\n");
        exit(1);
    }
    fprintf(f, "%s", output);
    fprintf(f, "%s", texto);
    fclose(f);
}

//============================================================
//          ALGORITMOS DE PAGINACION Y SEGMENTACION
//============================================================

/*
     Algoritmo Paginacion
*/
void *asignarEspacio_Paginacion(void *arg)
{

    struct Proceso *temp;
    temp = (struct Proceso *)arg;
    temp->pid = control_mem[3];

    // agrego el proceso actual a los procesos vivos
    int pos_proceso = agregarProceso(temp);

    // guardo los valores para accederlos más facil
    int cant_pags_proceso = procesos_mem[pos_proceso].cant_pags;
    int tiempo = procesos_mem[pos_proceso].tiempo;
    long int pid = procesos_mem[pos_proceso].pid;
    procesos_mem[pos_proceso].estado = Bloqueado;

    sleep(1);

    int pos = 0;
    bool asignado = false;

    // Array de rastreo de paginas

    int bitacoraPaginas[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    // pide el semáforo
    sem_wait(&sem_ready);
    procesos_mem[pos_proceso].estado = RegionCritica;

    sleep(1);

    // Valores para la bitácora
    char resultado[100];

    // Tamanho restante del tamanho original
    int tamanoRestante = cant_pags_proceso; // Ir restando el tamano restante con los espacios libres y si al final del for no es cero, se mata el proceso.
    int cant_pags_mem = control_mem[0];
    for (int i = 0; i < cant_pags_mem; i++)
    {

        if (readyQueue_mem[i].pid == -1)
        {
            tamanoRestante--;
            if (tamanoRestante == 0)
                break;
        }
    }
    if (tamanoRestante == 0)
    {
        // hay que agregarle un and y ponerle que el tamanhoRStante sea igual a cero, y hace un break.
        int counter = 0;
        for (int i = 0; i < cant_pags_mem; i++)
        {
            // Solo en los espacios que esten vacios
            if (counter < cant_pags_proceso)
            {
                if (readyQueue_mem[i].pid == -1)
                {
                    readyQueue_mem[i] = procesos_mem[pos_proceso];
                    bitacoraPaginas[counter] = i;
                    counter++;
                }
            }
        }
        asignado = true;
    }

    if (asignado)
    {
        // 3 Escribe en la bitácora

        // Convierte array a string
        char pag_usadas[255];
        join(pag_usadas, sizeof pag_usadas, bitacoraPaginas, sizeof(bitacoraPaginas) / sizeof(*bitacoraPaginas));

        sprintf(resultado, ": Proceso: Asignación. PID: %ld. Filas Ocupadas:%s.\n", pid, pag_usadas);
        escBitacora(resultado);

        // 4 Devuelve el semáforo
        sem_post(&sem_ready);
        procesos_mem[pos_proceso].estado = Ejecutando;

        // 5 Sleep

        printf("(Entró el proceso: %ld, pśginas: %s, tiempo: %d)\n", pid, pag_usadas, tiempo);

        sleep(tiempo);

        // 6 Pide el semáforo
        sem_wait(&sem_ready);

        // 8 Escribe en la bitácora

        // Convierte array a string
        join(pag_usadas, sizeof pag_usadas, bitacoraPaginas, sizeof(bitacoraPaginas) / sizeof(*bitacoraPaginas));

        sprintf(resultado, ": Proceso: Desasignación. PID: %ld. Filas Ocupadas:%s.\n", pid, pag_usadas);
        escBitacora(resultado);

        // 7  Devuelve  memoria
        for (int i = 0; i < 10; i++)
        {
            if (bitacoraPaginas[i] != -1)
            {
                readyQueue_mem[bitacoraPaginas[i]].pid = -1;
            }
        }
    }
    else
    {
        sprintf(resultado, ": Proceso: Muere. El hilo %ld de tamaño %d, murió. Motivo: No encontró espacio\n",
                pid, cant_pags_proceso);
        printf("-El hilo %ld de tamaño %d, murió porque no encontró espacio\n",
               pid, cant_pags_proceso);
        escBitacora(resultado);
        matarProceso(pid);
    }

    // elimino el proceso actual de los procesos vivos
    eliminarProceso(pid);

    // devuelve el semáforo
    sem_post(&sem_ready);

    return NULL;
}

/*
     Algoritmo Paginacion
*/
void *asignarEspacio_Segmentacion(void *arg)
{
    struct Proceso *temp;
    temp = (struct Proceso *)arg;
    temp->pid = control_mem[3];

    // agrego el proceso actual a los procesos vivos
    int pos_proceso = agregarProceso(temp);

    // guardo los valores para accederlos más facil
    int segmentos_proceso[5];

    memcpy(segmentos_proceso, procesos_mem[pos_proceso].segmentos, sizeof(procesos_mem[pos_proceso].segmentos));
    int tiempo = procesos_mem[pos_proceso].tiempo;
    long int pid = procesos_mem[pos_proceso].pid;
    procesos_mem[pos_proceso].estado = Bloqueado;

    sleep(1); 

    int pos = 0;
    bool ocupado = false;
    bool asignado = false;

    bool valido = false;

    // Array de rastreo de paginas
    int bitacoraSegmentos[15] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    // pide el semáforo
    sem_wait(&sem_ready);
    procesos_mem[pos_proceso].estado = RegionCritica;
 
    sleep(1); 

    // Valores para la bitácora
    char resultado[100];

    // REVISAR SI PUEDE METER EL PROCESO
    for (int i = 0; i < 5; i++)
    {
        if (segmentos_proceso[i] != 0 && !ocupado)
        {
            pos = 0;
            int tamano = segmentos_proceso[i];
            while (pos <= (control_mem[0] - tamano))
            {
                ocupado = false;

                // para saltar más rapido las que estan ocupadas

                while (readyQueue_mem[pos].pid != -1)
                {
                    pos = pos + 1;
                }

                for (int i = pos; i < (pos + tamano); i++)
                {
                    if (readyQueue_mem[i].pid != -1)
                    {
                        ocupado = true;
                        pos = i + 1;
                        break;
                    }
                }
                if (!ocupado && (pos <= control_mem[0] - tamano))
                {
                    asignado = true;
                    break;
                }
                else
                {
                    ocupado = true;
                    asignado = false;

                    break;
                }
            }
            if (!(pos <= (control_mem[0] - tamano)))
            {
                ocupado = true;
                break;
            }
        }
        else
        {
            break;
        }
    }

    // METER EL PROCESO
    int pos_bit = 0;

    if (asignado)
    {
        for (int i = 0; i < 5; i++)
        {
            if (segmentos_proceso[i] != 0 && !ocupado)
            {
                pos = 0;
                int tamano = segmentos_proceso[i];
                while (pos <= (control_mem[0] - tamano))
                { 
                    ocupado = false;


                    while (readyQueue_mem[pos].pid != -1)
                    {
                        pos = pos + 1;
                    }

                    for (int i = pos; i < (pos + tamano); i++)
                    {
                        if (readyQueue_mem[i].pid != -1)
                        {
                            ocupado = true;
                            pos = i + 1;
                            break;
                        }
                    }
                    if (!ocupado && (pos <= control_mem[0] - tamano))
                    {
                        for (int i = pos; i < (pos + tamano); i++)
                        {
                            readyQueue_mem[i] = procesos_mem[pos_proceso];
                            bitacoraSegmentos[pos_bit] = i;
                            pos_bit = pos_bit + 1;
                        }
                        asignado = true;
                        break;
                    }
                    else
                    {
                        ocupado = true;
                        asignado = false;
                        break;
                    }
                }
                if (!(pos <= (control_mem[0] - tamano)))
                {
                    ocupado = true;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    if (asignado)
    {
        // 3 Escribe en la bitácora

        // Convierte array a string
        char pag_usadas[255];
        join(pag_usadas, sizeof pag_usadas, bitacoraSegmentos, sizeof(bitacoraSegmentos) / sizeof(*bitacoraSegmentos));

        sprintf(resultado, ": Proceso: Asignación. PID: %ld. Filas Ocupadas:%s.\n", pid, pag_usadas);
        escBitacora(resultado);

        // 4 Devuelve el semáforo
        sem_post(&sem_ready);
        procesos_mem[pos_proceso].estado = Ejecutando;

        char tamanoSementos[255];
        join(tamanoSementos, sizeof tamanoSementos, segmentos_proceso, sizeof(segmentos_proceso) / sizeof(*segmentos_proceso));

        printf("(Entró el proceso: %ld, segmentos: %s, tiempo: %d)\n", pid, tamanoSementos, tiempo);

        // 5 Sleep
        sleep(tiempo);

        // 6 Pide el semáforo
        sem_wait(&sem_ready);

        // 8 Escribe en la bitácora

        // Convierte array a string
        join(pag_usadas, sizeof pag_usadas, bitacoraSegmentos, sizeof(bitacoraSegmentos) / sizeof(*bitacoraSegmentos));

        sprintf(resultado, ": Proceso: Desasignación. PID: %ld. Filas Ocupadas:%s.\n", pid, pag_usadas);
        escBitacora(resultado);

        // 7  Devuelve  memoria

        for (int i = 0; i < 15; i++)
        {
            if (bitacoraSegmentos[i] != -1)
            {
                readyQueue_mem[bitacoraSegmentos[i]].pid = -1;
            }
        }
    }
    else
    {

        for (int i = 0; i < control_mem[0]; i++)
        {
            if(readyQueue_mem[i].pid == pid)
            {
                readyQueue_mem[i].pid = -1;
            }
        }
        sprintf(resultado, ": Proceso: Muere. El proceso con PID %ld murió. Motivo: No encontró espacio\n",
                pid);
        printf("-El proceso con PID %ld, murió porque no encontró espacio\n",
               pid);
        escBitacora(resultado);
        matarProceso(pid);
    }

    // elimino el proceso actual de los procesos vivos
    eliminarProceso(pid);

    // devuelve el semáforo
    sem_post(&sem_ready);

    return NULL;
}

int main()
{
    // crea la llave
    key_t llave_ready, llave_control, llave_procesos;
    llave_ready = ftok(".", 'x');
    llave_control = ftok(".", 'a');
    llave_procesos = ftok(".", 'b');

    // shmget me retorna el identificador de la memoria compartida, si existe
    int ready_id = shmget(llave_ready, 0, 0666);
    int control_id = shmget(llave_control, 0, 0666);
    int procesos_id = shmget(llave_procesos, 0, 0666);

    if (ready_id == -1 || control_id == -1 || procesos_id == -1)
    {
        printf("No hay acceso a la memoria compartida\n");
    }
    else
    {
        // shmat se pega a la memoria compartida
        readyQueue_mem = (Proceso *)shmat(ready_id, (void *)0, 0);
        control_mem = (int *)shmat(control_id, (void *)0, 0);
        procesos_mem = (Proceso *)shmat(procesos_id, (void *)0, 0);

        if (readyQueue_mem == (void *)-1 || control_mem == (void *)-1 || procesos_mem == (void *)-1)
        {
            printf("No se puede apuntar a la memoria compartida\n");
        }
        else
        {

            int algoritmo = 0;
            while (algoritmo < 1 || algoritmo > 2)
            {
                printf("Algoritmos de asignación de memoria\n");
                printf("  1) Paginacion\n");
                printf("  2) Segmentacion\n");
                printf("______________________________________\n");
                printf("Seleccione el algoritmo a utilizar: ");
                scanf("%d", &algoritmo);

                if (algoritmo < 1 || algoritmo > 2)
                {
                    printf("Por favor, ingrese una de las opciones.\n");
                }
                printf("\n");
            }

            // inicializa el semáforo para hilos
            sem_init(&sem_ready, 0, 1);
            // inicializa el semáforo para los procesos
            sem_init(&sem_procesos, 0, 1);

            // Así debería comportarse, maomeno
            while (control_mem[1] == 1)
            { // mientras la memoria esté viva

                int espera = 0;
                pthread_t proceso;
                control_mem[3] = control_mem[3] + 1; // Suma de cantidad de procesos para mantener el PID

                if (algoritmo == 1)
                {
                    // Algoritmo de Paginacion

                    struct Proceso info_proceso_pag;

                    info_proceso_pag.cant_pags = getRandom(1, 10);
                    info_proceso_pag.tiempo = getRandom(20, 60);

                    pthread_create(&proceso, NULL, asignarEspacio_Paginacion, (void *)&info_proceso_pag);

                    espera = getRandom(30, 60);
                }

                else
                {
                    // Algoritmo de Segmentacion

                    struct Proceso info_proceso_seg;

                    int cant_segmentos = getRandom(1, 5);

                    // printf("Cant seg: %d \n", cant_segmentos);

                    for (int i = 0; i < 5; i++)
                    {
                        if (i < cant_segmentos)
                        {
                            info_proceso_seg.segmentos[i] = getRandom(1, 3);
                            sleep(1);
                        }
                        else
                        {
                            info_proceso_seg.segmentos[i] = 0;
                        }
                    }
                    info_proceso_seg.tiempo = getRandom(20, 60);

                    pthread_create(&proceso, NULL, asignarEspacio_Segmentacion, (void *)&info_proceso_seg);

                    espera = getRandom(15, 20) - cant_segmentos;
                }

                printf("Entrada de un nuevo proceso en %d segundos\n", espera);

                sleep(espera);
            }

            // destruye los semáforos
            sem_destroy(&sem_ready);
            sem_destroy(&sem_procesos);

            // se despega de la memoria compartida
            shmdt(readyQueue_mem);
            shmdt(control_mem);
            shmdt(procesos_mem);
        }
    }

    return 0;
}
