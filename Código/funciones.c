#include "funciones.h"
#include <pthread.h>

// La descripcion de cada funcion se encuentra en el archivo funciones.h con las cabeceras de las funciones.

//--------------------------------------------------------------------------------------------------------

double calculoEnergiaDepositada(float numero_celdas, float celda_impactada,
                                float posicion_celda_actual, float energia_potencial)
{
    // Formula para calcular la energia de impacto.
    double energia_depositada = (pow(10.0, 3.0) * energia_potencial) /
                                (numero_celdas * sqrt(fabs(celda_impactada - posicion_celda_actual) + 1));
    return energia_depositada;
}

//--------------------------------------------------------------------------------------------------------

double *calculoEnergiaDepositadaCeldas(double *celdas, int numero_celdas, int posicion_impacto,
                                       float energia)
{
    double energia_actual;
    // Se calcula y se suma la energia para cada una de las celdas.
    for (int i = 0; i < numero_celdas; ++i)
    {
        energia_actual = calculoEnergiaDepositada(numero_celdas, posicion_impacto,
                                                  i, energia);
        // No se suma si es una cantidad de energia despreciable.
        if (energia_actual >= (pow(10.0, -3.0) / numero_celdas))
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

void imprimirGrafico(double *energia_celdas, int numero_celdas, int celda_con_mayor_energia, pthread_t *id_hebras,
                     int *lineas_leidas, int numero_hebras)
{
    // Se imprime la cantidad de lineas leidas por cada hebra.
    for (int i = 0; i < numero_hebras; ++i)
    {
        printf("Hebra con el id: %lu, cantidad de lineas leídas: %d.\n", id_hebras[i], lineas_leidas[i]);
    }

    double mayor_energia = energia_celdas[celda_con_mayor_energia];
    double cantidad_o;
    for (int i = 0; i < numero_celdas; ++i)
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

    // Se convierte el puntero genérico a la estructura adecuada.
    struct DataHebra *data_hebras = (struct DataHebra *)arg;

    int posicion_impacto;
    int energia_particula;
    int posicion_id_hebra;

    // Se obtiene le id de la hebra.
    pthread_t tid = pthread_self();

    // Se otorga una posicion en el arreglo id_hebras segun el id obtenido.
    for (int i = 0; i < data_hebras->numero_hebras; ++i)
    {
        if (data_hebras->id_hebras[i] == tid)
        {
            posicion_id_hebra = i;
            break;
        }
    }

    // Mientras no se lea el final del archivo se continua leyendo chunks lineas.
    while (!feof(data_hebras->archivo_entrada))
    {
        // Se leen chunks lineas.
        for (int i = 0; i < data_hebras->numero_chunk; ++i)
        {
            // Si se intenta leer y se ha llegado al final del arvhivo se rompe la iteración.
            if (feof(data_hebras->archivo_entrada))
            {
                break;
            }

            // Se realiza un "wait" para el mutex lectura ya que es una SC.
            pthread_mutex_lock(&lectura);

            // Se suma uno a la cantidad de lineas leidos por la hebra.
            data_hebras->lineas_leidas[posicion_id_hebra]++;

            // Se lee los datos de una linea del archivo de entrada.
            fscanf(data_hebras->archivo_entrada, "%d", &posicion_impacto);
            fscanf(data_hebras->archivo_entrada, "%d", &energia_particula);

            // Se realiza un "signal" al mutex lectura al ya haber leído una línea.
            pthread_mutex_unlock(&lectura);

            // Se utiliza una condición para controlar que solo se escriba los calculas para la cantidad de particulas en el archivo de entrada.
            if (contador < data_hebras->numero_particulas)
            {
                // Se realiza un "wait" al mutex escritura ya que una hebra va a escribir en un arreglo compartido.
                pthread_mutex_lock(&escritura);

                data_hebras->celdas = calculoEnergiaDepositadaCeldas(data_hebras->celdas,
                                                                     data_hebras->numero_celdas, posicion_impacto, energia_particula);

                // Se suma uno al contador para "notificar" que se escribío el calculo de una particula leída.
                contador = contador + 1;

                // Se realizar un "signal" al mutex escritura al ya haber escrito en el arreglo compartido.
                pthread_mutex_unlock(&escritura);
            }
        }
    }

    // Se finaliza la hebra.
    pthread_exit(NULL);
}

//--------------------------------------------------------------------------------------------------------

void escrituraArchivoSalida(char *nombre_archivo_salida, double *celdas, int celda_mayor_energia,
                            int numero_celdas)
{
    // Se abre un archivo de salida en modo escritura.
    FILE *archivo_salida = fopen(nombre_archivo_salida, "w");

    // Se escribe primero la celda con mayor energia.
    fprintf(archivo_salida, "%d ", celda_mayor_energia - 1);
    fprintf(archivo_salida, "%f\n", celdas[celda_mayor_energia - 1]);

    // Se escriben las demas celdas.
    for (int i = 0; i < numero_celdas - 1; ++i)
    {
        fprintf(archivo_salida, "%d ", i);
        fprintf(archivo_salida, "%f\n", celdas[i]);
    }

    fprintf(archivo_salida, "%d ", numero_celdas - 1);
    fprintf(archivo_salida, "%f", celdas[numero_celdas - 1]);

    // se cierra el archivo de salida.
    fclose(archivo_salida);
}