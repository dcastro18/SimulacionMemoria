#ifndef PROCESO_SEGMENTACION_H
#define PROCESO_SEGMENTACION_H


typedef struct Proceso_Segmentacion
{
    long int pid;
    int tiempo;
    int segmentos[10];
    Estado estado;
    
} Proceso_Segmentacion;


#endif // PROCESO_SEGMENTACION_H