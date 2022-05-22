#ifndef PROCESO_H
#define PROCESO_H

typedef enum Estado {Fuera=-1, Bloqueado=0, Ejecutando=1, RegionCritica=2}Estado;

typedef struct Proceso
{
    long int pid;
    int tiempo;
    int espacios;
    Estado estado;
} Proceso;


#endif // PROCESO_H