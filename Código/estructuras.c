#include <stdio.h>
#include <pthread.h>

//--------------------------------------------------------------------------------------------------------

// Descripción: struct utilizado para agrupar datos que luego se enviaran a las hebras.
struct DataHebra
{
    FILE *archivo_entrada;
    int numero_celdas;
    int numero_chunk;
    int numero_particulas;
    int numero_hebras;
    double *celdas;
    // Datos para identicar cuantas lecturas hizo cada una de las hebras.
    pthread_t *id_hebras;
    int *lineas_leidas;
};