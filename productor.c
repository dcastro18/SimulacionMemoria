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