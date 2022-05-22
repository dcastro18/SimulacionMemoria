#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "proceso.h"

//============================================================
//                  ALGORITMO DE BITÁCORA
//============================================================
// Crea o limmpia el archivo de bitácora
void crearBitacora(){
    //FILE *f = fopen ("/home/evelio/MemoriaCompartida-SO/bitacora.txt", "a");
    FILE *f = fopen ("bitacora.txt", "w");
    if(f == NULL){
        printf("Error al abrir el archivo\n");
        exit(1);
    }
    fclose(f);
}

int main()
{
    int espacios = 0;
    while(espacios <= 0){
        printf("Ingrese el número de paginas o espacios de memoria: ");
        scanf("%d", &espacios);

        if(espacios <= 0){
            printf("Por favor, ingrese un número positivo.\n\n");
        }
    }

    //crea las llaves para las memorias compartidas
    key_t llave_ready, llave_control, llave_procesos;
    llave_ready = ftok(".",'x');
    llave_control = ftok(".",'a');
    llave_procesos = ftok(".",'b');

    // shmget retorna el identificador de la memoria compartida
    int ready_id = shmget(llave_ready, espacios*sizeof(Proceso), 0666|IPC_CREAT);
    int control_id = shmget(llave_control, 2*sizeof(int), 0666|IPC_CREAT);
    int procesos_id = shmget(llave_procesos, espacios*sizeof(Proceso), 0666|IPC_CREAT);


    //Este código sirve como ejemplo para ver datos extra sobre la memoria compartida
    /*struct shmid_ds shmid_ds;
    printf ("The segment size = %ld\n", shmid_ds.shm_segsz);
    */

    if(ready_id == -1 || control_id == -1 || procesos_id == -1){
        printf("No se pudo crear la memoria compartida\n");
    }else{
        // shmat se pega a la memoria compartida
        Proceso * readyqueue_mem = (Proceso *) shmat(ready_id,(void*)0,0);
        int * control_mem = (int *) shmat(control_id,(void*)0,0);

        if(readyqueue_mem == (void *)-1 || control_mem == (void *)-1){
            printf("No se puede apuntar a la memoria compartida\n");
        }else{
            control_mem[0] = espacios;   //cantidad de espacios disponibles de la memoria compartida
            control_mem[1] = 1;             //el programa corre
            control_mem[2] = 0;             //cantidad de procesos
            control_mem[3] = 0;             // cantidad de proceso creados

            for(int i=0; i<espacios; i++){
                readyqueue_mem[i].pid = -1;
                //readyqueue_mem[i].estado = -1;
            }

            crearBitacora();

            printf("¡Memoria compartida creada con éxito!\n");
            

            //se despega de la memoria compartida
            shmdt(readyqueue_mem);
            shmdt(control_mem);
        }
    }


    return 0;
}
