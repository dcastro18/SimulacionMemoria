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
#include "proceso.h"
// Variables Globales
// Puntero a la memoria de proceso
Proceso * mem_address;
// Puntero al control de la memoria del proceso (ID de la memoria en la que esta guardado el proceso)
int * control_address;
// Puntero al estado del proceso
Proceso * estados_address;
/*
    The <semaphore.h> header defines the sem_t type, used in performing semaphore operations. 
    The semaphore may be implemented using a file descriptor, in which case applications are able to open up 
    at least a total of OPEN_MAX files and semaphores. 
*/
// Semaforo de estados y ready
sem_t sem_ready, sem_estados;
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
    // Pide el semáforo
    sem_wait(&sem_estados); /*
    The sem_wait() function locks the semaphore referenced by sem by performing a semaphore lock operation on that semaphore. 
    If the semaphore value is currently zero, then the calling thread will not return from the call to sem_wait() until it either locks the semaphore or the call is interrupted by a signal.
    */

    int pos = control_address[2];                   //cantidad de procesos vivos
    estados_address[pos].pid = proceso->pid;
    estados_address[pos].tamano = proceso->tamano;
    estados_address[pos].tiempo = proceso->tiempo;
    
    control_address[2] = control_address[2] + 1;    //incremento el contador de procesos vivos

    // devuelve el semáforo
    sem_post(&sem_estados);

    return pos;
}

/*
    Elimina un proceso de la memoria de procesos vivos y reordena
*/
void eliminarProceso(long int idProceso){
    // pide el semáforo
    sem_wait(&sem_estados);

    int pos=0;
    while(estados_address[pos].pid != idProceso)
        pos++;

    for(int i=pos; i<(control_address[2]-1); i++)
        estados_address[i] = estados_address[i+1];

    control_address[2] = control_address[2] - 1;

    // devuelve el semáforo
    sem_post(&sem_estados);
}
// Genera bitacora
// Esta funcion se debe de llamar en los algoritmos de paginacion y segmentacion 
void generadorDeBitacora(const char *texto){
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


// Algoritmo de Paginacion
// Algoritmo de Segmentacion

int main()
{
    


    return 0;
}