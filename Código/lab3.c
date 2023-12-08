#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include "funciones.h"

// Los mutex se ocupan como variable global

// mutex para EM de lectura, para que dos o mas hebras no lean la misma linea;
pthread_mutex_t lectura;

// mutex para EM de escritura en arreglos compartidos.
pthread_mutex_t escritura;

// contador utilizado para tener constantemente la informacion de a cuantas particulas se le
// a calculado la energia de impacto en las celdas.
int contador = 0;

// Se crean arreglos globales.
double *celdas;
pthread_t *id_hebras;
int *lineas_leidas;

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

    // Se lee la primera linea para tener el dato de la cantidad de particulas en el archivo de entrada.
    int numero_particulas;
    fscanf(archivo_entrada, "%d", &numero_particulas);

    // Se le asigna memoria al arreglo que almacenara las ids de las hebras.
    id_hebras = (pthread_t *)malloc(sizeof(pthread_t) * numero_hebras);

    // Se inicializa arreglo para almacenar energia de las celdas.
    celdas = (double *)malloc(sizeof(double) * numero_celdas);
    // Se establece el valor de cada celda en 0.
    for (int i = 0; i < numero_celdas; ++i)
    {
        celdas[i] = 0;
    }

    // Se asigna memoria para el arreglo contador de lineas leidas por cada hebra.
    lineas_leidas = (int *)malloc(sizeof(int) * numero_hebras);
    // Se establece cada posicion del arreglo en 0.
    for (int i = 0; i < numero_hebras; ++i)
    {
        lineas_leidas[i] = 0;
    }

    // Se crea un struct DataHebra para agrupas los datos a enviar a las hebras.
    struct DataHebra data_hebras;
    data_hebras.archivo_entrada = archivo_entrada;
    data_hebras.numero_hebras = numero_hebras;
    data_hebras.numero_celdas = numero_celdas;
    data_hebras.numero_chunk = numero_chunks;
    data_hebras.numero_particulas = numero_particulas;
    data_hebras.id_hebras = id_hebras;
    data_hebras.lineas_leidas = lineas_leidas;
    data_hebras.celdas = celdas;

    // Se inicia el mutex de lectura.
    pthread_mutex_init(&lectura, NULL);

    // Se inicia el mutex de escritura.
    pthread_mutex_init(&escritura, NULL);

    // Se crean las hebras segun la cantidad ingresada asignada en numero_hebras.
    pthread_t *tids = (pthread_t *)malloc(sizeof(pthread_t) * numero_hebras);
    for (int i = 0; i < numero_hebras; ++i)
    {
        // A cada hebra se le pasa la funcion lecturaArchivo y el struct data_hebras.
        pthread_create(&tids[i], NULL, lecturaArchivo, (void *)&data_hebras);
        // Se almacena el id de la hebra en el arreglo id_hebras.
        id_hebras[i] = tids[i];
    }

    // Se espera a que las hebras terminen.
    for (int i = 0; i < numero_hebras; i++)
    {
        pthread_join(tids[i], NULL);
    }

    // Se obtiene la celda con mayor energia.
    int celda_mayor_energia = celdaMayorEnergia(data_hebras.celdas, data_hebras.numero_celdas);

    // Si se escribio la flag D se imprime por pantalla el grafico.
    if (imprimir == 1)
    {
        imprimirGrafico(data_hebras.celdas, data_hebras.numero_celdas, celda_mayor_energia, id_hebras, lineas_leidas, numero_hebras);
    }

    // Se crea el archivo de salida y se escriben los datos en el.
    escrituraArchivoSalida(nombre_archivo_salida, data_hebras.celdas, celda_mayor_energia, data_hebras.numero_celdas);

    // Se libera la memoria de los arreglos utilizados.
    free(celdas);
    free(lineas_leidas);
    free(id_hebras);

    return 0;
}