
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#include "proceso.h"

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
    

    return 0;
}
