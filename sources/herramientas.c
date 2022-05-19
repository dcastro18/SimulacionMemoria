#include "imports.h"


/*####################################################################################################################*/
/*###############################################    Memoria    ######################################################*/
/*####################################################################################################################*/

int solicitar_mem()
{
    int shm_id;

    printf("Solicitando espacio de memoria...");
    shm_id = shmget(IPC_PRIVATE, SEGMENTSIZE, IPC_CREAT | IPC_EXCL | SEGMENTPERM);
    if (shm_id == -1)
    {
        perror("ERROR\n");
        exit(1);
    }
    printf("OK. ID: %d\n", shm_id);
    return shm_id;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void * asociar_mem(int shm_id)
{
    void * shm_addr;

    printf("Asociando a espacio de memoria %d...", shm_id);
    shm_addr = shmat(shm_id, NULL, 0);
    if (!shm_addr)
    {
        printf("ERROR\n");
        exit(1);
    }
    printf("OK\n");

    return shm_addr;
}

/*####################################################################################################################*/
/*###############################################   Semaforos   ######################################################*/
/*####################################################################################################################*/

void * solicitar_sem(char * nombre_sem)
{
    sem_t * sem;

    printf("Solicitando semaforo...");
    sem = sem_open (nombre_sem , O_CREAT|O_EXCL , S_IRUSR|S_IWUSR , 1);

    if (!sem == SEM_FAILED)
        printf("creado...");
    else if (errno == EEXIST)
    {
        sem = sem_open (nombre_sem, 0);
        printf("abierto...");
    }
    printf("OK\n");
    return (void *) sem;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void bloquear_sem(void * sem_ref)
{
    sem_t * sem = (sem_t *) sem_ref;
    int result = 0;

    printf("Bloqueando semaforo...");

    result = sem_wait(sem);
    if (result == -1)
    {
        printf("ERROR\n");
        exit(2);
    }
    printf("OK\n");
}

/*--------------------------------------------------------------------------------------------------------------------*/

void desbloquear_sem(void * sem_ref)
{
    sem_t * sem = (sem_t *) sem_ref;
    int result;

    printf("Desbloqueando semaforo...");

    result = sem_post(sem);
    if (result == -1)
    {
        printf("ERROR\n");
        exit(2);
    }
    printf("OK\n");
}

/*--------------------------------------------------------------------------------------------------------------------*/

void cerrar_sem(void * sem_ref)
{
    sem_t * sem = (sem_t *) sem_ref;
    int result = 0;

    printf("Cerrando semaforo...");

    result = sem_close (sem);
    if (result == -1)
    {
        printf("ERROR\n");
        exit(2);
    }

    printf("clausurado...");

    result = sem_unlink (SEM_NAME);
    if (result == -1)
    {
        printf("ERROR\n");
        exit(2);
    }
    printf("OK\n");
}


/*####################################################################################################################*/
/*###############################################   Generales   ######################################################*/
/*####################################################################################################################*/

/*--------------------------------------------------------------------------------------------------------------------*/
void registrar_accion(char * file_name, char * registro){
    FILE *fptr;

    //Se abre con "a" para realizar un append en el archivo
    fptr = fopen(file_name, "a");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }

    fprintf(fptr,"\n%s", registro);
    fclose(fptr);
}

/*--------------------------------------------------------------------------------------------------------------------*/
int random_number(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    srand(time(NULL));
    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void save_int(int num, char * file_name)
{
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(file, "%d", num);
    fclose(file);
}

/*--------------------------------------------------------------------------------------------------------------------*/

int read_int(char * file_name)
{
    FILE *file;
    char buff[255];

    file = fopen(file_name, "r");
    fscanf(file, "%s", buff);
    fclose(file);

    return atoi(buff);
}

/*--------------------------------------------------------------------------------------------------------------------*/

char* get_time(){
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    return asctime(timeinfo);
}