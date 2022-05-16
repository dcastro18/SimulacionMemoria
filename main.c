#include "paginacion.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

paginacionInfo pagesMemory[PAGE_COUNT_MAX];// El número de páginas almacenadas en la memoria (según la hora del último acceso a la memoria: ordenadas de pequeña a grande, el último accedido en la primera posición, etc.
paginacionInfo pagesInpuntPage[PAGE_COUNT_MAX*2];// El número de páginas que se pueden ingresar
int memory_size;// Número de páginas en memoria
int input_size;// Número de direcciones de página

// Estructura de Proceso
typedef struct PCB {
    int PID;
    int burst;
    int priority;
    int tat;
    int wt;
} PCB;



int main(int argc, char const *argv[])
{

    
    int n, opcion;

    do
    {
        printf( "\n   Bienvenido al menú");
        printf( "\n   1. " );
        printf( "\n   2. ");
        printf( "\n   3. ." );
        printf( "\n\n   Introduzca opción que desee (1-3): ");

        scanf( "%d", &opcion );

        /* Inicio del anidamiento */

        pthread_t mainThread;

        switch ( opcion )
        {
            case 1:

                break;

            case 2:
                break;

            case 3:
                break;
        }

        /* Fin del anidamiento */

    } while ( opcion < 0 || opcion > 3 );

    return 0;
}

