#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main()
{
    //crea la llave.ini
    key_t readyqueque_mem, llave_control, llave_procesos;
    readyqueque_mem = ftok(".",'x');
    llave_control = ftok(".",'a');
    llave_procesos = ftok(".",'b');

    // shmget me retorna el identificador de la memoria compartida
    int ready_id = shmget(readyqueque_mem, 0, 0);
    int control_id = shmget(llave_control, 0, 0);
    int procesos_id = shmget(llave_procesos, 0, 0);

    if(ready_id == -1 || control_id == -1 || procesos_id == -1){
        printf("No hay acceso a la memoria compartida\n");
    }else{
        printf("Eliminando...\n");

        // destruye la memoria compartida 
        shmctl(ready_id, IPC_RMID, NULL);
        shmctl(procesos_id, IPC_RMID, NULL); 

        // Esto es para que el productor deje de producir y muera
        int *control_mem = (int*) shmat(control_id,(void*)0,0);
        control_mem[1] = 0;

        //sleep(4);   //espera al productor

        //destruye la memoria de control
        shmctl(control_id, IPC_RMID, NULL);

        printf("¡Memoria compartida eliminada con éxito!\n");
    }


    return 0;
}
