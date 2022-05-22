#ifndef PROCESO_H
#define PROCESO_H

typedef enum Estado {Fuera=-1, Bloqueado=0, Ejecutando=1, RegionCritica=2}Estado; 

typedef struct Proceso
{
	long int pid;
    int cant_pags; 
    int tiempo;
    Estado estado;
    int seg[5];

} Proceso;


#endif // LINEA_H