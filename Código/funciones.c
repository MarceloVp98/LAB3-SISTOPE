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

double *calculoEnergiaDepositadaCeldas(double *celdas, int total_celdas, int posicion_impacto,
                                       float energia)
{
    double energia_actual;
    // Se calcula y se suma la energia para cada una de las celdas.
    for (int i = 0; i < total_celdas; ++i)
    {
        energia_actual = calculoEnergiaDepositada(total_celdas, posicion_impacto,
                                                  i, energia);
        if (energia_actual >= (pow(10.0, -3.0) / total_celdas))
        {
            celdas[i] = celdas[i] + energia_actual;
        }
    }
    return celdas;
}

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
            if (feof(data_hebras->archivo_entrada))
            {
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

            if (contador < data_hebras->numero_particulas)
            {
                // Se realiza un "wait" al mutex escritura ya que una hebra va a escribir en un arreglo compartido.
                pthread_mutex_lock(&escritura);

                data_hebras->celdas = calculoEnergiaDepositadaCeldas(data_hebras->celdas,
                                                                     data_hebras->numero_celdas, posicion_impacto, energia_particula);

                contador = contador + 1;

                // Se realizar un "signal" al mutex escritura al ya haber escrito en el arreglo compartido.
                pthread_mutex_unlock(&escritura);
            }
        }
    }

    pthread_exit(NULL);
}

//--------------------------------------------------------------------------------------------------------

void escrituraArchivoSalida(char *nombre_archivo_salida, double *celdas, int celda_mayor_energia,
                            int numero_celdas)
{
    FILE *archivo_salida = fopen(nombre_archivo_salida, "w");

    // Se escribe primero la celda con mayor energia
    fprintf(archivo_salida, "%d ", celda_mayor_energia - 1);
    fprintf(archivo_salida, "%f\n", celdas[celda_mayor_energia - 1]);

    for (int i = 0; i < numero_celdas - 1; ++i)
    {
        fprintf(archivo_salida, "%d ", i);
        fprintf(archivo_salida, "%f\n", celdas[i]);
    }

    fprintf(archivo_salida, "%d ", numero_celdas - 1);
    fprintf(archivo_salida, "%f", celdas[numero_celdas - 1]);

    fclose(archivo_salida);
}