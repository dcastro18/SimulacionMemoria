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
Proceso * mem_address;
int * control_address;
Proceso * estados_address;
sem_t sem_ready, sem_estados;

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
    sem_wait(&sem_estados);

    int pos = control_address[2];                   //cantidad de procesos vivos
    estados_address[pos].pid = proceso->pid;
    estados_address[pos].cant_pags = proceso->cant_pags;
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
//          ALGORITMOS DE ProcesoCION Y SEGMENTACION
//============================================================

/*
     Algoritmo Paginacion
*/
void * asignarEspacio_Paginacion(void * arg){
    
    struct Proceso * temp;
    temp = (struct Proceso *)arg;
    temp->pid = control_address[3];

    // agrego el proceso actual a los procesos vivos
    int pos_estado = agregarProceso(temp);
    //guardo los valores para accederlos más facil
    int cant_pags_proceso = estados_address[pos_estado].cant_pags;
    int tiempo = estados_address[pos_estado].tiempo;
    long int pid = estados_address[pos_estado].pid;
    estados_address[pos_estado].estado = Bloqueado;

    //printf("Bloqueado... ");
    sleep(1);      //para notarlo en el espia ¡NO BORRAR!

    int pos = 0;
    bool ocupado;
    bool asignado = false;

    // Array de rastreo de paginas

    int bitacoraPaginas[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

    // pide el semáforo
    sem_wait(&sem_ready);
    estados_address[pos_estado].estado = RegionCritica;

    //printf("RC... ");
    sleep(1);         //para notarlo en el espia ¡NO BORRAR!

    //Valores para la bitácora
    char resultado[100];


    // control_address[0] = cantidad de lineas
    ocupado = false;
    // para saltar más rapido las que estan ocupadas
    //while(mem_address[pos].pid != -1)
        //pos++;
    // Tamanho restante del tamanho original
    int tamanoRestante = cant_pags_proceso; // Ir restando el tamano restante con los espacios libres y si al final del for no es cero, se mata el proceso.
    int cant_pags_mem = control_address[0];
    for(int i=0; i < cant_pags_mem; i++){

       if(mem_address[i].pid == -1){
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
                if(mem_address[i].pid == -1){
                    mem_address[i] = estados_address[pos_estado];
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
        estados_address[pos_estado].estado = Ejecutando;
        
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
                mem_address[bitacoraPaginas[i]].pid = -1;
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

/*
     Algoritmo Segmentacion
*/
void * asignarEspacio_Segmentacion(void * arg){
    
    struct Proceso * temp;
    temp = (struct Proceso *)arg;
    temp->pid = control_address[3];

    int pos_estado = agregarProceso(temp);
    
    // Iguala los segmentos dados por el random
    int segmentos_proceso[5];
    for (int i =0 ; i < 5 ; i++) {
        segmentos_proceso[i] = estados_address[pos_estado].seg[i];
        printf("Prueba iguala en segmentos Debe de igualar: %d ---- Lllega como: %d\n", segmentos_proceso[i], estados_address[pos_estado].seg[i]);
    }
    
    int tiempo = estados_address[pos_estado].tiempo;
    long int pid = estados_address[pos_estado].pid;
    estados_address[pos_estado].estado = Bloqueado;

    //printf("Bloqueado... ");
    sleep(1);      //para notarlo en el espia ¡NO BORRAR!

    int pos = 0;
    bool ocupado;
    bool asignado = false;

    // Array de rastreo de paginas

    int bitacoraPaginas[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

    // pide el semáforo
    sem_wait(&sem_ready);
    estados_address[pos_estado].estado = RegionCritica;

    //printf("RC... ");
    sleep(1);         //para notarlo en el espia ¡NO BORRAR!

    //Valores para la bitácora
    char resultado[100];


    // control_address[0] = cantidad de lineas
    ocupado = false;
    int cant_segmentos_proceso = sizeof(segmentos_proceso)/sizeof(segmentos_proceso[0]);
    
    for(int i = 0;i<cant_segmentos_proceso;i++ ) {
        int tamano = segmentos_proceso[i];

        while(pos <= (control_address[0] - tamano)){        // control_address[0] = cantidad de lineas
            ocupado = false;
            // para saltar más rapido las que estan ocupadas
            while(mem_address[pos].pid != -1)
                pos++;

            for(int i=pos; i<(pos + tamano); i++){
                if(mem_address[i].pid != -1){
                    ocupado = true;
                    pos = i + 1;
                    break;
                }
            }

            if(!ocupado && (pos <= control_address[0] - tamano)){
                asignado = true;
                break;
            }
        }
        if(ocupado) {
            break;
        }
        pos = 0;

    }
    if(!ocupado) {
        for(int i = 0;i<cant_segmentos_proceso;i++ ) {
            int tamano = segmentos_proceso[i];
            while(pos <= (control_address[0] - tamano)){        // control_address[0] = cantidad de lineas
                ocupado = false;

                // para saltar más rapido las que estan ocupadas
                while(mem_address[pos].pid != -1)
                    pos++;

                for(int i=pos; i<(pos + tamano); i++){
                    if(mem_address[i].pid != -1){
                        ocupado = true;
                        pos = i + 1;
                        break;
                    }
                }

                if(!ocupado && (pos <= control_address[0] - tamano)){
                    for(int i=pos; i<(pos + tamano); i++){
                        // Revisar
                        mem_address[i] = estados_address[pos_estado];
                    }
                    asignado = true;
                    break;
                }
            }
            pos = 0;

        }

    }
    
    if(asignado){
    	// 3 Escribe en la bitácora
        // Convierte array a string
        char seg_usados[255];
	    join(seg_usados, sizeof seg_usados, segmentos_proceso, sizeof(segmentos_proceso) / sizeof(*segmentos_proceso));

    	sprintf(resultado, ": Proceso: Asignación. PID: %ld. Segmentos Random Generados:%s.\n", pid, seg_usados);
    	escBitacora(resultado);

        // 4 Devuelve el semáforo
        sem_post(&sem_ready);
        estados_address[pos_estado].estado = Ejecutando;
        
	   // 5 Sleep
        //sleep(1);//para notarlo en el espia ¡NO BORRAR!
        //char seg_usados[255];
	    join(seg_usados, sizeof seg_usados, segmentos_proceso, sizeof(segmentos_proceso) / sizeof(*segmentos_proceso));
        printf("(Entró %ld: Segmentos Random Generados %d, tiempo %d)\n", pid, seg_usados, tiempo);
        
        sleep(tiempo);

        // 6 Pide el semáforo
        sem_wait(&sem_ready);

    	// 8 Escribe en la bitácora
        // Convierte array a string
        
        //char seg_usados[255];
	    join(seg_usados, sizeof seg_usados, segmentos_proceso, sizeof(segmentos_proceso) / sizeof(*segmentos_proceso));

    	sprintf(resultado, ": Proceso: Desasignación. PID: %ld. Segmentos Random Generados:%s.\n", pid, seg_usados);
    	escBitacora(resultado);

        // 7  Devuelve  memoria

        for(int i=0; i<10; i++){
            if (bitacoraPaginas[i] != -1){
                mem_address[bitacoraPaginas[i]].pid = -1;
            }
            
        }

    }else{
        sprintf(resultado, ": Proceso: Muere. El hilo %ld de tamaño %d, murió. Motivo: No encontró espacio\n",
             pid, segmentos_proceso);
        printf("-El hilo %ld de tamaño %d, murió porque no encontró espacio\n",
                 pid, segmentos_proceso);
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
    key_t llave_mem, llave_control, llave_estados;
    llave_mem = ftok(".",'x');
    llave_control = ftok(".",'a');
    llave_estados = ftok(".",'b');

    // shmget me retorna el identificador de la memoria compartida, si existe
    int mem_id = shmget(llave_mem, 0, 0666);
    int control_id = shmget(llave_control, 0, 0666);
    int estados_id = shmget(llave_estados, 0, 0666);

    if(mem_id == -1 || control_id == -1 || estados_id == -1){
        printf("No hay acceso a la memoria compartida\n");
    }else{
        // shmat se pega a la memoria compartida
        mem_address = (Proceso*) shmat(mem_id, (void*) 0, 0);
        control_address = (int*) shmat(control_id, (void*) 0, 0);
        estados_address = (Proceso*) shmat(estados_id, (void*) 0, 0);

        if(mem_address == (void*)-1 || control_address == (void*)-1 
            || estados_address == (void*)-1){
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
            // inicializa el semáforo para los estados
            sem_init(&sem_estados, 0, 1);

            // Así debería comportarse, maomeno
            while(control_address[1] == 1){         //mientras la memoria esté viva
                
                pthread_t proceso;
                struct Proceso info_proceso_pag;
                // Suma id
                control_address[3] = control_address[3] + 1; // Suma de cantidad de procesos para mantener el PID
                info_proceso_pag.cant_pags = getRandom(1, 10);
                info_proceso_pag.tiempo = getRandom(20, 60);
                
                
                if(algoritmo == 1){
                    pthread_create(&proceso, NULL, asignarEspacio_Paginacion, (void *)&info_proceso_pag);
                }else if (algoritmo == 2){
                    // Pone los random en cada array de segmentos (Tentativo)
                    int cant_segmentos = getRandom(1, 5);
                    int segmentos[cant_segmentos];
                
                    for (int i = 0; i<cant_segmentos ;++i) {
                        int tamano_segmento = getRandom(1, 3);
                        printf("Random de Segmento Generado: %d\n",tamano_segmento);
                        // Iguala segmento
                        info_proceso_pag.seg[i] = tamano_segmento;
                        printf("Random de Segmento Igualado: %d\n",info_proceso_pag.seg[i]);

                    }
                    pthread_create(&proceso, NULL, asignarEspacio_Segmentacion, (void *)&info_proceso_pag);
                }
                    

                
                int espera = getRandom(15, 25);
                printf("Nuevo hilo en %d segundos\n", espera);

                sleep(espera);
            }


            // destruye los semáforos
            sem_destroy(&sem_ready);
            sem_destroy(&sem_estados);

            //se despega de la memoria compartida
            shmdt(mem_address);
            shmdt(control_address);
            shmdt(estados_address);
        }
    }


    return 0;
}
