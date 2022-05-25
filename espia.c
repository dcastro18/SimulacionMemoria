#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "proceso.h"

int main()
{
    // crea la llave
    key_t readyqueque_mem, llave_control, llave_procesos;
    readyqueque_mem = ftok(".", 'x');
    llave_control = ftok(".", 'a');
    llave_procesos = ftok(".", 'b');

    // shmget me retorna el identificador de la memoria compartida, si existe
    int ready_id = shmget(readyqueque_mem, 0, 0666);
    int control_id = shmget(llave_control, 0, 0666);
    int procesos_id = shmget(llave_procesos, 0, 0666);

    if (ready_id == -1 || control_id == -1 || procesos_id == -1)
    {
        printf("No hay acceso a la memoria compartida\n");
    }
    else
    {
        // shmat se pega a la memoria compartida
        Proceso *readyqueque_mem = (Proceso *)shmat(ready_id, (void *)0, 0);
        int *control_mem = (int *)shmat(control_id, (void *)0, 0);
        Proceso *procesos_mem = (Proceso *)shmat(procesos_id, (void *)0, 0);

        // Arreglos extra
       

        if (readyqueque_mem == (void *)-1 || control_mem == (void *)-1 || procesos_mem == (void *)-1)
        {
            printf("No se puede apuntar a la memoria compartida\n");
        }
        else
        {

            int cant_lineas = control_mem[0];

            printf("======= ESTADO DE PROCESOS ========\n");
            printf("PID                 Estado\n");
            printf("-------------------------------\n");
            for (int i = 0; i < control_mem[2]; i++)
            {
                printf("%-20ld", procesos_mem[i].pid);
                switch (procesos_mem[i].estado)
                {
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
                case Muerto:
                    printf("Muerto");
                    break;
                }
                printf("\n-------------------------------\n");
            }

            printf("\n====== MEMORIA READY =======\n\n");
            printf("N°       PID\n");
            printf("----------------------------\n");

            // printf("Data written in memory: %s\n", str);
            for (int i = 0; i < cant_lineas; i++)
            {
                printf("%-9d", i);

                if (readyqueque_mem[i].pid == -1)
                {
                    printf("%-20s", "-");
                }
                else
                {
                    printf("%-20ld", readyqueque_mem[i].pid);
                }

                printf("\n----------------------------\n");
            }

            // se despega de la memoria compartida
            shmdt(readyqueque_mem);
            shmdt(control_mem);
            shmdt(procesos_mem);
        }
    }

    return 0;
}
