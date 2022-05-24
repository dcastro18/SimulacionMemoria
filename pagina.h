#ifndef PAGINA_H
#define PAGINA_H

//typedef enum Estado {Fuera=-1, Bloqueado=0, Ejecutando=1, RegionCritica=2}Estado; 

typedef struct Pagina
{
	long int pid;
    int cant_pags; 
    int tiempo;
    Estado estado;
    
} Pagina;


#endif // PAGINA_H