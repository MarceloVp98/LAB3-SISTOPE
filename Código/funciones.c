#include "funciones.h"
#include <pthread.h>

//--------------------------------------------------------------------------------------------------------

double calculoEnergiaDepositada(float total_celdas, float celda_impactada,
                                float posicion_celda_actual, float energia_potencial)
{
    // Formula para calcular la energia de impacto.
    double energia_depositada = (pow(10.0, 3.0) * energia_potencial) /
                                (total_celdas * sqrt(fabs(celda_impactada - posicion_celda_actual) + 1));
    return energia_depositada;
}

//--------------------------------------------------------------------------------------------------------

/* double *calculoEnergiaDepositadaCeldas(particulas *particulas, int total_celdas,
                                       int cantidad_particulas)
{
    // Se crea el arreglo para almacenar la energia de las celdas.
    double *energia_celdas = (double *)malloc(sizeof(double) * total_celdas);
    if (energia_celdas == NULL)
    {
        // Manejo de error si no se pudo asignar memoria.
        return NULL;
    }
    // Se asigna 0.0 como energia a cada celda.
    for (int i = 0; i < total_celdas; ++i)
    {
        energia_celdas[i] = 0.0;
    }
    double energia_actual;
    // Se calcula y se suma la energia para cada una de las celdas.
    for (int i = 0; i < cantidad_particulas; ++i)
    {
        for (int j = 0; j < total_celdas; ++j)
        {
            energia_actual = calculoEnergiaDepositada(total_celdas, particulas[i].posicion_impacto,
                                                      j, particulas[i].energia);
            if (energia_actual >= (pow(10.0, -3.0) / total_celdas))
            {
                energia_celdas[j] = energia_celdas[j] + energia_actual;
            }
        }
    }
    return energia_celdas;
} */

//--------------------------------------------------------------------------------------------------------

int celdaMayorEnergia(double *energia_celdas, int total_celdas)
{
    double mayor_energia = 0;
    int posicion;
    // Busqueda de la celda con mayor energia.
    for (int i = 0; i < total_celdas; ++i)
    {
        if (energia_celdas[i] > mayor_energia)
        {
            mayor_energia = energia_celdas[i];
            posicion = i;
        }
    }
    return posicion + 1;
}

//--------------------------------------------------------------------------------------------------------

void imprimirGrafico(double *energia_celdas, int total_celdas, int celda_con_mayor_energia)
{
    double mayor_energia = energia_celdas[celda_con_mayor_energia];
    double cantidad_o;
    for (int i = 0; i < total_celdas; ++i)
    {
        printf("%d  %f |", i, energia_celdas[i]);
        // Se calcula la cantidad de O a imprimir segun la energia de la celda.
        // Se divide en 2 para que no sean tantas.
        cantidad_o = ((energia_celdas[i] / mayor_energia) * 100) / 2;
        for (double j = 0.0; j < cantidad_o; ++j)
        {
            printf("O");
        }
        printf("\n");
    }
}

//--------------------------------------------------------------------------------------------------------

void *lecturaArchivo(void *arg)
{
    // Convierte el puntero genérico a la estructura adecuada.
    struct DataHebra *data_hebras = (struct DataHebra *)arg;

    int posicion_impacto;
    int energia_particula;

    while (!feof(data_hebras->archivo_entrada))
    {
        for (int i = 0; i < data_hebras->numero_chunk; ++i)
        {
            if(feof(data_hebras->archivo_entrada)){
                break;
            }

            pthread_t tid = pthread_self();
            printf("Soy la hebra con ID: %lu\n", tid);

            // Se realiza un "wait" para el mutex lectura ya que es una SC.
            pthread_mutex_lock(&lectura);

            fscanf(data_hebras->archivo_entrada, "%d", &posicion_impacto);
            fscanf(data_hebras->archivo_entrada, "%d", &energia_particula);

            // Se realiza un "signal" al mutex lectura al ya haber leído una línea.
            pthread_mutex_unlock(&lectura);

            // Se realiza un "wait" al mutex escritura ya que una hebra va a escribir en un arreglo compartido.
            pthread_mutex_lock(&escritura);

            data_hebras->particulas[contador].posicion_impacto = posicion_impacto;
            data_hebras->particulas[contador].energia = energia_particula;

            contador = contador + 1;

            // Se realizar un "signal" al mutex escritura al ya haber escrito en el arreglo compartido.
            pthread_mutex_unlock(&escritura);

        }
    }

    pthread_exit(NULL);
}