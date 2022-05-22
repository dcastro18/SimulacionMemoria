#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//#include "linea.h"
#include "proceso_paginacion.h"

int main()
{
    //crea la llave
    key_t readyqueque_mem, llave_control, llave_procesos;
    readyqueque_mem = ftok(".",'x');
    llave_control = ftok(".",'a');
    llave_procesos = ftok(".",'b');

    // shmget me retorna el identificador de la memoria compartida, si existe
    int ready_id = shmget(readyqueque_mem, 0, 0666);
    int control_id = shmget(llave_control, 0, 0666);
    int procesos_id = shmget(llave_procesos, 0, 0666);

    if(ready_id == -1 || control_id == -1 || procesos_id == -1){
        printf("No hay acceso a la memoria compartida\n");
    }else{
        // shmat se pega a la memoria compartida
        Proceso * mem_address = (Proceso *) shmat(ready_id, (void*)0, 0);
        int * control_mem = (int*) shmat(control_id, (void*)0, 0);
        Proceso * procesos_address = (Proceso *) shmat(procesos_id, (void*)0, 0);

        if(mem_address == (void*)-1 || control_mem == (void*)-1 
            || procesos_address == (void*)-1){
            printf("No se puede apuntar a la memoria compartida\n");
        }else{
            int cant_lineas = control_mem[0];

            printf("======= Procesos vivos ========\n");
            printf("PID                 Estado\n");
            printf("-------------------------------\n");
            for(int i=0; i<control_mem[2]; i++){
                printf("%-20ld", procesos_address[i].pid);
                switch(procesos_address[i].estado){
                    case Fuera:
                        printf("Fuera");
                        break;
                    case Bloqueado:
                        printf("Bloqueado");
                        break;
                    case Ejecutando:
                        printf("Ejecutando");
                        break;
                    case RegionCritica:
                        printf("En Región Crítica");
                        break;
                }
                printf("\n-------------------------------\n");
            }

            printf("\n====== Memoria Ready =======\n");
            printf("N°       PID\n");
            printf("----------------------------\n");

            //printf("Data written in memory: %s\n", str);
            for(int i=0; i<cant_lineas; i++){
                printf("%-9d", i);

                if(mem_address[i].pid == -1){
                    printf("%-20s", "-");
                }else{
                    printf("%-20ld", mem_address[i].pid);
                }

                printf("\n----------------------------\n");
            }

            //se despega de la memoria compartida
            shmdt(mem_address);
            shmdt(control_mem);
            shmdt(procesos_address);
        }
    }


    return 0;
}
