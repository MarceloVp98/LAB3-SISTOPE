#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>

// Los mutex se ocupan como variable global

// mutex para EM de lectura y que dos o mas hebras lean la misma linea;
pthread_mutex_t lectura;

// mutex para EM de escritura.
pthread_mutex_t escritura;

int contador = 0;

struct particula *particulas;

double *celdas;

#include "funciones.h"

int main(int argc, char *argv[])
{
    int opt, numero_celdas = 0, numero_hebras = 0,
             numero_chunks = 0, imprimir = 0;
    char *nombre_archivo_entrada = NULL, *nombre_archivo_salida = NULL;

    // Se utiliza getopt para captar las banderas y datos ingresados.
    while ((opt = getopt(argc, argv, "N:H:i:o:c:D")) != -1)
    {
        switch (opt)
        {
        case 'N':
            numero_celdas = atoi(optarg);
            break;

        case 'H':
            numero_hebras = atoi(optarg);
            break;

        case 'i':
            nombre_archivo_entrada = optarg;
            break;

        case 'o':
            nombre_archivo_salida = optarg;
            break;

        case 'c':
            numero_chunks = atoi(optarg);
            break;

        case 'D':
            imprimir = 1;
            break;
        }
    }

    // Si no se ingresa archivo de entrada y/o salida.
    if (nombre_archivo_entrada == NULL || nombre_archivo_salida == NULL)
    {
        perror("Debe ingresar el nombre del archivo de entrada y de salida.\n");
        return 1;
    }

    // Se valida que el numero de celdas sea un numero positivo.
    if (numero_celdas < 0)
    {
        printf("Debe ingresar un numero positivo para la bandera N.\n");
        return 1;
    }

    // Se abre el archivo de entrada.
    FILE *archivo_entrada = fopen(nombre_archivo_entrada, "r");

    int numero_particulas;
    fscanf(archivo_entrada, "%d", &numero_particulas);

    // Se inicializa un arreglo de struct particula para pasarselo a las hebras.
    particulas = (struct particula *)malloc(sizeof(struct particula) * numero_particulas);

    // Se inicializa arreglo para almacenar energia de las celdas.
    celdas = (double *)malloc(sizeof(double) * numero_celdas);
    for (int i = 0; i < numero_celdas; ++i)
    {
        celdas[i] = 0;
    }

    // Se crea un struct DataHebra para agrupas los datos a enviar a las hebras.
    struct DataHebra data_hebras;
    data_hebras.archivo_entrada = archivo_entrada;
    data_hebras.numero_celdas = numero_celdas;
    data_hebras.numero_chunk = numero_chunks;
    data_hebras.numero_particulas = numero_particulas;
    data_hebras.particulas = particulas;
    data_hebras.celdas = celdas;

    // Se inicia el mutex de lectura.
    pthread_mutex_init(&lectura, NULL);

    // Se inicia el mutex de escritura.
    pthread_mutex_init(&escritura, NULL);

    // Se crean las hebras segun la cantidad ingresada asignada en numero_hebras.
    pthread_t tids[5];
    for (int i = 0; i < numero_hebras; ++i)
    {
        pthread_create(&tids[i], NULL, lecturaArchivo, (void *)&data_hebras);
    }

    // Se espera a que las hebras terminen.
    for (int i = 0; i < numero_hebras; i++)
    {
        pthread_join(tids[i], NULL);
    }

    /* // Imprimir el arreglo de partículas
    for (int i = 0; i < numero_particulas; ++i)
    {
        printf("Partícula %d: posicion_impacto=%d, energia=%d\n", i + 1, particulas[i].posicion_impacto, particulas[i].energia);
    } */

    // Liberar memoria
    free(particulas);
    free(celdas);

    return 0;
}