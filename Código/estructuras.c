#include <stdio.h>

//--------------------------------------------------------------------------------------------------------

// Descripción: struct para almacenar los datos de una particula.
struct particula
{
    int posicion_impacto;
    float energia;
};

//--------------------------------------------------------------------------------------------------------

// Descripción: struct utilizado para agrupar datos que luego se enviaran a las hebras.
struct DataHebra
{
    FILE *archivo_entrada;
    int numero_celdas;
    int numero_chunk;
    int numero_particulas;
    double * celdas;
};