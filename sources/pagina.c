#include "../headers/pagina.h"

void agregar_pagina(int * n_paginas, int * n_paginas_disp, Pagina * memoria, int pag_logica)
{
    memoria[*n_paginas].pag_logica = pag_logica;
    memoria[*n_paginas].thread_id = NULL;
    memoria[*n_paginas].estado = DISPONIBLE;
    (*n_paginas)++;
    (*n_paginas_disp)++;
}