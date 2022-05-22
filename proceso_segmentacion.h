#ifndef PROCESO_SEGMENTACION_H
#define PROCESO_SEGMENTACION_H

typedef enum Estado {Fuera=-1, Bloqueado=0, Ejecutando=1, RegionCritica=2}Estado;

typedef struct Proceso_Segmentacion
{
    long int pid;
    int tiempo;
    int segmentos[];
    Estado estado;
    
} Proceso_Segmentacion;


#endif // PROCESO_SEGMENTACION_H