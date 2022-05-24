#ifndef LINEA_H
#define LINEA_H

typedef struct Linea
{
    long int pid;
    int segmentos[5]; 
    int cant_pags; 
    int tiempo;
    Estado estado;

} Linea;


#endif // LINEA_H