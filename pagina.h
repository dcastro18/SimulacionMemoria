#ifndef PAGINA_H
#define PAGINA_H

typedef enum Estado {Fuera=-1, Bloqueado=0, Ejecutando=1, RegionCritica=2}Estado; 

typedef struct Pagina
{
	long int id_proceso;
    int tamano_proceso;
    int tiempo_proceso;
    Estado estado_proceso;
    
} Pagina;


#endif // PAGINA_H