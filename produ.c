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
<<<<<<< HEAD


//#include "linea.h"
=======
#include <stdio.h>


>>>>>>> Revvro
#include "proceso.h"


// Variables globales
Proceso * mem_address;
int * control_address;
Proceso * estados_address;
sem_t sem_ready, sem_estados;

<<<<<<< HEAD
=======
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

>>>>>>> Revvro
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
<<<<<<< HEAD
    estados_address[pos].tamano = proceso->tamano;
=======
    estados_address[pos].cant_pags = proceso->cant_pags;
>>>>>>> Revvro
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
<<<<<<< HEAD
//          ALGORITMOS DE PAGINACION Y SEGMENTACION
=======
//          ALGORITMOS DE ProcesoCION Y SEGMENTACION
>>>>>>> Revvro
//============================================================

/*
     Algoritmo Paginacion
*/
<<<<<<< HEAD
void * first_fit(void * arg){
    
    struct Proceso * temp;
    temp = (struct Proceso *)arg;
    temp->pid = pthread_self();
=======
void * asignarEspacio_Paginacion(void * arg){
    
    struct Proceso * temp;
    temp = (struct Proceso *)arg;
    temp->pid = control_address[3];
>>>>>>> Revvro

    // agrego el proceso actual a los procesos vivos
    int pos_estado = agregarProceso(temp);
    //guardo los valores para accederlos más facil
<<<<<<< HEAD
    int tamano = estados_address[pos_estado].tamano;
=======
    int cant_pags_proceso = estados_address[pos_estado].cant_pags;
>>>>>>> Revvro
    int tiempo = estados_address[pos_estado].tiempo;
    long int pid = estados_address[pos_estado].pid;
    estados_address[pos_estado].estado = Bloqueado;

    //printf("Bloqueado... ");
    sleep(1);      //para notarlo en el espia ¡NO BORRAR!

    int pos = 0;
    bool ocupado;
<<<<<<< HEAD
    bool insertado = false;
=======
    bool asignado = false;

    // Array de rastreo de paginas

    int bitacoraPaginas[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
>>>>>>> Revvro

    // pide el semáforo
    sem_wait(&sem_ready);
    estados_address[pos_estado].estado = RegionCritica;

    //printf("RC... ");
    sleep(1);         //para notarlo en el espia ¡NO BORRAR!

    //Valores para la bitácora
    char resultado[100];

<<<<<<< HEAD
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
                mem_address[i] = estados_address[pos_estado];
            }
            insertado = true;
            break;
        }
    }

    if(insertado){
    	//Escribe en la bitácora
    	sprintf(resultado, ": Proceso: Asignación. Hilo: %ld. Filas Ocupadas:%d-%d. Tamaño %d. Tiempo %d\n", pid, pos, pos+tamano-1, tamano, tiempo);
    	escBitacora(resultado);
        // devuelve el semáforo
        sem_post(&sem_ready);

        estados_address[pos_estado].estado = Ejecutando;
        
	   //printf("Ejecutando...\n");
        sleep(1);//para notarlo en el espia ¡NO BORRAR!
	        
        printf("(Entró %ld: tamaño %d, tiempo %d)\n", pid, tamano, tiempo);
        
        sleep(tiempo);// "Ejecuta"

        // pide el semáforo
        sem_wait(&sem_ready);

    	//Escribe en la bitácora
    	sprintf(resultado, ": Proceso: Desasignación. Hilo: %ld. Filas Ocupadas:%d-%d. Tamaño %d. Tiempo %d\n",
             pid, pos, pos+tamano-1, tamano, tiempo);
    	escBitacora(resultado);

        // devuelve los recursos
        for(int i=pos; i<(pos + tamano); i++){
            mem_address[i].pid = -1;
        }

    }else{
        sprintf(resultado, ": Proceso: Defunción. El hilo %ld de tamaño %d, murió. Motivo: No encontró amor\n",
             pid, tamano);
        printf("-El hilo %ld de tamaño %d, murió porque no encontró amor\n",
                 pid, tamano);
        escBitacora(resultado);
    }

    // elimino el proceso actual de los procesos vivos
    eliminarProceso(pid);

    // devuelve el semáforo
    sem_post(&sem_ready);

    return NULL;
}



void * best_fit(void * arg){
    struct Proceso * temp;
    temp = (struct Proceso *)arg;
    temp->pid = pthread_self();

    // agrego el proceso actual a los procesos vivos
    int pos_estado = agregarProceso(temp);
    //guardo los valores para accederlos más facil
    int tamano = estados_address[pos_estado].tamano;
    int tiempo = estados_address[pos_estado].tiempo;
    long int pid = estados_address[pos_estado].pid;
    estados_address[pos_estado].estado = Bloqueado;

    //printf("Bloqueado... ");
    sleep(1);      //para notarlo en el espia ¡NO BORRAR!
    
    //Valores para la bitácora
    char resultado[100];

    //Valores para el Bestfit
    int iPos = -1;
    int fPos = -1;
    int espacio = 0;

    int pos = 0;
    bool ocupado;
    bool insertado = false;

    // pide el semáforo
    sem_wait(&sem_ready);
    estados_address[pos_estado].estado = RegionCritica;

    //printf("RC... ");
    sleep(1);         //para notarlo en el espia ¡NO BORRAR!

    while(pos <= (control_address[0] - tamano)){//control_address[0] = cantidad de lineas
        ocupado = false;

        // para saltar más rapido las que estan ocupadas
        while(mem_address[pos].pid != -1){
            pos++;
        }

        for(int i=pos; i<(pos + tamano); i++){
            if(mem_address[i].pid != -1){
                ocupado = true;
                pos = i + 1;
                break;
            }
        }

        if(!ocupado && (pos <= control_address[0] - tamano)){
    	    int cantEspacio = 0;
    	    int contador = pos;
    	    while(mem_address[contador].pid == -1){
    		  contador = contador + 1;
    		  cantEspacio = cantEspacio + 1;
    	    }
    	    if (pos>iPos && pos>fPos){
        		if(espacio == 0) {
        		    espacio = cantEspacio;
        		    iPos = pos;
        		    fPos = iPos + cantEspacio;
        		}else{ 
        		    if(cantEspacio >= tamano && cantEspacio < espacio){
        			espacio = cantEspacio;
        			iPos = pos;
        			fPos = iPos + cantEspacio;
        		    }
        		}
    	    }
    	}

    	pos = pos + 1; 
    }

    if(iPos>=0){
        for(int i=iPos; i<(iPos + tamano); i++){
            mem_address[i] = estados_address[pos_estado];
        }
        insertado = true;
    }

    if(insertado){
    	// Escribe en bitácora
    	sprintf(resultado, ": Proceso: Asignación. Hilo: %ld. Filas Ocupadas:%d-%d. Tamaño %d. Tiempo %d\n",
             pid, iPos, iPos+tamano-1, tamano, tiempo);
    	escBitacora(resultado);
        
	   // devuelve el semáforo
        sem_post(&sem_ready);

        estados_address[pos_estado].estado = Ejecutando;

        //printf("Ejecutando...\n");
        sleep(1);         //para notarlo en el espia ¡NO BORRAR!
        printf("(Entró %ld: tamaño %d, tiempo %d)\n", pid, tamano, tiempo);
        sleep(tiempo);              // "Ejecuta"

        // pide el semáforo
        sem_wait(&sem_ready);

    	// Escribe en bitácora
    	sprintf(resultado, ": Proceso: Desasignación. Hilo: %ld. Filas Ocupadas:%d-%d. Tamaño %d. Tiempo %d\n",
             pid, iPos, iPos+tamano-1, tamano, tiempo);
    	escBitacora(resultado);

        // devuelve los recursos
        for(int i=iPos; i<(iPos + tamano); i++){
            mem_address[i].pid = -1;
        }

    }else{
        sprintf(resultado, ": Proceso: Defunción. El hilo %ld de tamaño %d, murió. Motivo: No encontró amor\n",
             pid, tamano);
        printf("-El hilo %ld de tamaño %d, murió porque no encontró amor\n", pid, tamano);
        escBitacora(resultado);
    }

    // elimino el proceso actual de los procesos vivos
    eliminarProceso(pid);

    // devuelve el semáforo
    sem_post(&sem_ready);

    return NULL;
}


void * worst_fit(void * arg){
    struct Proceso * temp;
    temp = (struct Proceso *)arg;
    temp->pid = pthread_self();

    // agrego el proceso actual a los procesos vivos
    int pos_estado = agregarProceso(temp);
    //guardo los valores para accederlos más facil
    int tamano = estados_address[pos_estado].tamano;
    int tiempo = estados_address[pos_estado].tiempo;
    long int pid = estados_address[pos_estado].pid;
    estados_address[pos_estado].estado = Bloqueado;

    //printf("Bloqueado... ");
    sleep(1);      //para notarlo en el espia ¡NO BORRAR!

    //Valores para la bitácora
    char resultado[100];

    //Valores para el Worstfit
    int iPos = -1;
    int fPos = -1;
    int espacio = 0;

    int pos = 0;
    bool ocupado;
    bool insertado = false;

    // pide el semáforo
    sem_wait(&sem_ready);
    estados_address[pos_estado].estado = RegionCritica;

    //printf("RC... ");
    sleep(1);         //para notarlo en el espia ¡NO BORRAR!

    while(pos <= (control_address[0] - tamano)){//control_address[0] = cantidad de lineas
        ocupado = false;

        // para saltar más rapido las que estan ocupadas
        while(mem_address[pos].pid != -1){
            pos++;
        }

        for(int i=pos; i<(pos + tamano); i++){
            if(mem_address[i].pid != -1){
                ocupado = true;
                pos = i + 1;
                break;
            }
        }

        if(!ocupado && (pos <= control_address[0] - tamano)){
    	    int cantEspacio = 0;
    	    int contador = pos;
    	    while(mem_address[contador].pid == -1){
                contador = contador + 1;
                cantEspacio = cantEspacio + 1;
    	    }
    	    if (pos>iPos && pos>fPos){
        		if(espacio == 0) {
        		    espacio = cantEspacio;
        		    iPos = pos;
        		    fPos = iPos + cantEspacio;
        		}else{ 
        		    if(cantEspacio >= tamano && cantEspacio > espacio){
                        printf("aqui\n");
                        espacio = cantEspacio;
            			iPos = pos;
            			fPos = iPos + cantEspacio;
        		    }
        		}
    	    }
    	}

    	pos = pos + 1; 
    }

    if(iPos>=0){
        for(int i=iPos; i<(iPos + tamano); i++){
            mem_address[i] = estados_address[pos_estado];
        }
        insertado = true;
    }

    if(insertado){
    	//Escribe en bitácora
    	sprintf(resultado, ": Proceso: Asignación. Hilo: %ld. Filas Ocupadas:%d-%d. Tamaño %d. Tiempo %d\n",
             pid, iPos, iPos+tamano-1, tamano, tiempo);
    	escBitacora(resultado);
	
        // devuelve el semáforo
        sem_post(&sem_ready);

        estados_address[pos_estado].estado = Ejecutando;

        //printf("Ejecutando...\n");
        sleep(1);         //para notarlo en el espia ¡NO BORRAR! 
        printf("(Entró %ld: tamaño %d, tiempo %d)\n", pid, tamano, tiempo);
        
        sleep(tiempo);              // "Ejecuta"

        // pide el semáforo
        sem_wait(&sem_ready);

    	//Escribe en bitácora
    	sprintf(resultado, ": Proceso: Desasignación. Hilo: %ld. Filas Ocupadas:%d-%d. Tamaño %d. Tiempo %d\n",
             pid, iPos, iPos+tamano-1, tamano, tiempo);
    	escBitacora(resultado);

        // devuelve los recursos
        for(int i=iPos; i<(iPos + tamano); i++){
            mem_address[i].pid = -1;
        }

    }else{
        sprintf(resultado, ": Proceso: Defunción. El hilo %ld de tamaño %d, murió. Motivo: No encontró amor\n", pid, tamano);
        printf("-El hilo %ld de tamaño %d, murió porque no encontró amor\n", pid, tamano);
=======

    // control_address[0] = cantidad de lineas
    ocupado = false;
    // para saltar más rapido las que estan ocupadas
    //while(mem_address[pos].pid != -1)
        //pos++;
    // Tamanho restante del tamanho original
    int tamanoRestante = cant_pags_proceso; // Ir restando el tamano restante con los espacios libres y si al final del for no es cero, se mata el proceso.
    int cant_pags_mem = control_address[0];
    for(int i=0; i < cant_pags_mem; i++){
        /*
        if(ocupado == true){

            if(mem_address[i].pid == -1){
                ocupado = false;    
                tamanoRestante--;
                if (tamanoRestante == 0)
                    break;
            }

        }
        else if(mem_address[i].pid == -1){
            ocupado = false;
            tamanoRestante--;
            if (tamanoRestante == 0)
                break;
        }
        else{
            ocupado = true;    
        }
        */

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
>>>>>>> Revvro
        escBitacora(resultado);
    }

    // elimino el proceso actual de los procesos vivos
    eliminarProceso(pid);

    // devuelve el semáforo
    sem_post(&sem_ready);

    return NULL;
}

<<<<<<< HEAD

=======
>>>>>>> Revvro
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
<<<<<<< HEAD
            int cant_lineas = control_address[0];
=======
>>>>>>> Revvro

            int algoritmo = 0;
            while(algoritmo < 1 || algoritmo > 2){
                printf("Algoritmos de asignación de memoria\n");
<<<<<<< HEAD
                printf("  1) Paginacion\n");
=======
                printf("  1) Procesocion\n");
>>>>>>> Revvro
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
<<<<<<< HEAD
                pthread_t proceso;
                struct Proceso info_proceso;

                info_proceso.tamano = getRandom(1, 10);
                info_proceso.tiempo = getRandom(20, 60);
=======
                
                pthread_t proceso;
                struct Proceso info_proceso_pag;
                // Suma id
                control_address[3] = control_address[3] + 1; // Suma de cantidad de procesos para mantener el PID
                info_proceso_pag.cant_pags = getRandom(1, 10);
                info_proceso_pag.tiempo = getRandom(20, 60);
>>>>>>> Revvro

                switch(algoritmo){
                    // Algoritmo de Paginacion
                    case 1:
<<<<<<< HEAD
                        pthread_create(&proceso, NULL, first_fit, (void *)&info_proceso);
                        break;
                    // Algoritmo de Segmentacion
                    case 2:
                        pthread_create(&proceso, NULL, best_fit, (void *)&info_proceso);
=======
                        pthread_create(&proceso, NULL, asignarEspacio_Paginacion, (void *)&info_proceso_pag);
                        break;
                    // Algoritmo de Segmentacion
                    case 2:
                        pthread_create(&proceso, NULL, asignarEspacio_Paginacion, (void *)&info_proceso_pag);
>>>>>>> Revvro
                        break;
                }


<<<<<<< HEAD
                int espera = getRandom(10, 20);
                //int espera = getRandom(30, 60);
                printf("Nuevo hilo en %d segundos\n", espera);
=======
                int espera = getRandom(15, 25);
                printf("Nuevo hilo en %d segundos\n", espera);

>>>>>>> Revvro
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
