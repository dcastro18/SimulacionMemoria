#include "paginacion.h"

paginacionInfo pagesMemory[PAGE_COUNT_MAX];// El número de páginas almacenadas en la memoria (según la hora del último acceso a la memoria: ordenadas de pequeña a grande, el último accedido en la primera posición, etc.
paginacionInfo pagesInpuntPage[PAGE_COUNT_MAX*2];// El número de páginas que se pueden ingresar
int memory_size;// Número de páginas en memoria
int input_size;// Número de direcciones de página

