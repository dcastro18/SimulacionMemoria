#ifndef PROCESO_H
#define PROCESO_H

typedef enum Estado {Fuera=-1, Bloqueado=0, Ejecutando=1, RegionCritica=2, Muerto=3}Estado; 

typedef struct Proceso
{
	long int pid;
    int cant_pags; 
    int tiempo;
    int segmentos[5];
    Estado estado;
} Proceso;

#endif // PROCESO_H

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