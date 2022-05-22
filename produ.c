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
Proceso * readyQueue_mem;
int * control_mem;
Proceso * procesos_mem;
sem_t sem_ready, sem_procesos;

// Auxiliar para obtener la cantidad de procesos en el Queue
int getQueueSize(int queue[10]) {
    int i = 0;
    int counter = 0;
    for (i = 0 ; i < 10 ; i++) {
        if (i != 0){
            break;
        }
        else
            counter++;
    }
    return counter;
}

char *join(char *dest, size_t size, const int *array, size_t count) {
    if (size == 0) {
        return NULL;
    }
    if (size == 1) {
       dest[0] = '\0';
       return dest;
    }
    size_t pos = 0;
    dest[pos++] = '[';
    dest[pos] = '\0';
    for (size_t i = 0; pos < size && i < count; i++) { 
        int len = snprintf(dest + pos, size - pos, "%d%s",
                           array[i], (i + 1 < count) ? ", " : "]");
        if (len < 0)
            return NULL;
        pos += len;
    }
    return dest;
}

/**
    Retorna un numero entero aleatorio entre [min, max]
*/
int getRandom(int min, int max){
    srand(time(0));
    int numero = (rand() % (max - min + 1)) + min;
    return numero;
}

/*
    Agrega un proceso a la memoria de procesos vivos
    Retorna la posición donde lo metió
*/
int agregarProceso(Proceso * proceso){
    // pide el semáforo
    sem_wait(&sem_procesos);

    int pos = control_mem[2];                   //cantidad de procesos vivos
    procesos_mem[pos].pid = proceso->pid;
    procesos_mem[pos].cant_pags = proceso->cant_pags;
    procesos_mem[pos].tiempo = proceso->tiempo;
    
    control_mem[2] = control_mem[2] + 1;    //incremento el contador de procesos vivos

    // devuelve el semáforo
    sem_post(&sem_procesos);

    return pos;
}

/*
    Elimina un proceso de la memoria de procesos vivos y reordena
*/
void eliminarProceso(long int idProceso){
    // pide el semáforo
    sem_wait(&sem_procesos);

    int pos=0;
    while(procesos_mem[pos].pid != idProceso)
        pos++;

    for(int i=pos; i<(control_mem[2]-1); i++)
        procesos_mem[i] = procesos_mem[i+1];

    control_mem[2] = control_mem[2] - 1;

    // devuelve el semáforo
    sem_post(&sem_procesos);
}


//============================================================
//                  ALGORITMO DE BITÁCORA
//============================================================
void escBitacora(const char *texto){
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char output[128];
	strftime(output, 128, "%d/%m/%Y %H:%M:%S", tlocal);

	//FILE *f = fopen ("/home/evelio/MemoriaCompartida-SO/bitacora.txt", "a");
    FILE *f = fopen ("bitacora.txt", "a");
	if(f == NULL){
		printf("Error al abrir el archivo\n");
		exit(1);
	}
	fprintf (f, "%s", output);
	fprintf (f, "%s", texto);
	fclose(f);
}


//============================================================
//          ALGORITMOS DE PAGINACION Y SEGMENTACION
//============================================================

/*
     Algoritmo Paginacion
*/
void * asignarEspacio_Paginacion(void * arg){
    
    struct Proceso * temp;
    temp = (struct Proceso *)arg;
    temp->pid = control_mem[3];

    // agrego el proceso actual a los procesos vivos
    int pos_proceso = agregarProceso(temp);
    //guardo los valores para accederlos más facil
    int cant_pags_proceso = procesos_mem[pos_proceso].cant_pags;
    int tiempo = procesos_mem[pos_proceso].tiempo;
    long int pid = procesos_mem[pos_proceso].pid;
    procesos_mem[pos_proceso].estado = Bloqueado;

    //printf("Bloqueado... ");
    sleep(1);      //para notarlo en el espia ¡NO BORRAR!

    int pos = 0;
    bool asignado = false;

    // Array de rastreo de paginas

    int bitacoraPaginas[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

    // pide el semáforo
    sem_wait(&sem_ready);
    procesos_mem[pos_estado].estado = RegionCritica;

    //printf("RC... ");
    sleep(1);         //para notarlo en el espia ¡NO BORRAR!

    //Valores para la bitácora
    char resultado[100];

 
    // Tamanho restante del tamanho original
    int tamanoRestante = cant_pags_proceso; // Ir restando el tamano restante con los espacios libres y si al final del for no es cero, se mata el proceso.
    int cant_pags_mem = control_mem[0];
    for(int i=0; i < cant_pags_mem; i++){

       if(readyQueue_mem[i].pid == -1){
            ocupado = false;    
            tamanoRestante--;
            if (tamanoRestante == 0)
                break;
        }
        
    }
    if(tamanoRestante == 0){
        // hay que agregarle un and y ponerle que el tamanhoRStante sea igual a cero, y hace un break.
        int counter = 0;
        for(int i=0; i < cant_pags_mem; i++){
            // Solo en los espacios que esten vacios
            if (counter < cant_pags_proceso){
                if(readyQueue_mem[i].pid == -1){
                    readyQueue_mem[i] = procesos_mem[pos_estado];
                    bitacoraPaginas[counter] = i;
                    counter++;   
                }

            }
        }
        asignado = true;
    }
    
    if(asignado){
    	// 3 Escribe en la bitácora
        // Convierte array a string
        char pag_usadas[255];
        join(pag_usadas, sizeof pag_usadas, bitacoraPaginas, sizeof(bitacoraPaginas) / sizeof(*bitacoraPaginas));

    	sprintf(resultado, ": Proceso: Asignación. PID: %ld. Filas Ocupadas:%s.\n", pid, pag_usadas);
    	escBitacora(resultado);

        // 4 Devuelve el semáforo
        sem_post(&sem_ready);
        procesos_mem[pos_estado].estado = Ejecutando;
        
	   // 5 Sleep
        //sleep(1);//para notarlo en el espia ¡NO BORRAR!
	        
        printf("(Entró %ld: tamaño %d, tiempo %d)\n", pid, cant_pags_proceso, tiempo);
        
        sleep(tiempo);

        // 6 Pide el semáforo
        sem_wait(&sem_ready);

    	// 8 Escribe en la bitácora
        // Convierte array a string
        
        join(pag_usadas, sizeof pag_usadas, bitacoraPaginas, sizeof(bitacoraPaginas) / sizeof(*bitacoraPaginas));

    	sprintf(resultado, ": Proceso: Desasignación. PID: %ld. Filas Ocupadas:%s.\n", pid, pag_usadas);
    	escBitacora(resultado);

        // 7  Devuelve  memoria

        for(int i=0; i<10; i++){
            if (bitacoraPaginas[i] != -1){
                readyQueue_mem[bitacoraPaginas[i]].pid = -1;
            }
            
        }

    }else{
        sprintf(resultado, ": Proceso: Muere. El hilo %ld de tamaño %d, murió. Motivo: No encontró espacio\n",
             pid, cant_pags_proceso);
        printf("-El hilo %ld de tamaño %d, murió porque no encontró espacio\n",
                 pid, cant_pags_proceso);
        escBitacora(resultado);
    }

    // elimino el proceso actual de los procesos vivos
    eliminarProceso(pid);

    // devuelve el semáforo
    sem_post(&sem_ready);

    return NULL;
}

int main()
{
    //crea la llave
    key_t llave_ready, llave_control, llave_procesos;
    llave_ready = ftok(".",'x');
    llave_control = ftok(".",'a');
    llave_procesos = ftok(".",'b');

    // shmget me retorna el identificador de la memoria compartida, si existe
    int ready_id = shmget(llave_ready, 0, 0666);
    int control_id = shmget(llave_control, 0, 0666);
    int procesos_id = shmget(llave_procesos, 0, 0666);

    if(ready_id == -1 || control_id == -1 || procesos_id == -1){
        printf("No hay acceso a la memoria compartida\n");
    }else{
        // shmat se pega a la memoria compartida
        readyQueue_mem = (Proceso*) shmat(ready_id, (void*) 0, 0);
        control_mem = (int*) shmat(control_id, (void*) 0, 0);
        procesos_mem = (Proceso*) shmat(procesos_id, (void*) 0, 0);

        if(readyQueue_mem == (void*)-1 || control_mem == (void*)-1 
            || procesos_mem == (void*)-1){
            printf("No se puede apuntar a la memoria compartida\n");
        }else{

            int algoritmo = 0;
            while(algoritmo < 1 || algoritmo > 2){
                printf("Algoritmos de asignación de memoria\n");
                printf("  1) Paginacion\n");
                printf("  2) Segmentacion\n");
                printf("______________________________________\n");
                printf("Seleccione el algoritmo a utilizar: ");
                scanf("%d", &algoritmo);

                if(algoritmo < 1 || algoritmo > 2){
                    printf("Por favor, ingrese una de las opciones.\n");
                }
                printf("\n");
            }

            // inicializa el semáforo para hilos
            sem_init(&sem_ready, 0, 1);
            // inicializa el semáforo para los procesos
            sem_init(&sem_procesos, 0, 1);

            // Así debería comportarse, maomeno
            while(control_mem[1] == 1){         //mientras la memoria esté viva
                
                pthread_t proceso;
                struct Proceso info_proceso_pag;
                // Suma id
                control_mem[3] = control_mem[3] + 1; // Suma de cantidad de procesos para mantener el PID
                info_proceso_pag.cant_pags = getRandom(1, 10);
                info_proceso_pag.tiempo = getRandom(20, 60);

                switch(algoritmo){
                    // Algoritmo de Paginacion
                    case 1:
                        pthread_create(&proceso, NULL, asignarEspacio_Paginacion, (void *)&info_proceso_pag);
                        break;
                    // Algoritmo de Segmentacion
                    case 2:
                        pthread_create(&proceso, NULL, asignarEspacio_Paginacion, (void *)&info_proceso_pag);
                        break;
                }

                int espera = getRandom(15, 25);
                printf("Nuevo hilo en %d segundos\n", espera);

                sleep(espera);
            }


            // destruye los semáforos
            sem_destroy(&sem_ready);
            sem_destroy(&sem_procesos);

            //se despega de la memoria compartida
            shmdt(readyQueue_mem);
            shmdt(control_mem);
            shmdt(procesos_mem);
        }
    }


    return 0;
}
