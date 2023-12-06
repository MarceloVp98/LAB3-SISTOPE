#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "estructuras.c"

// Variables grobales definidas en lab3.c.
#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <pthread.h>

extern pthread_mutex_t lectura;
extern pthread_mutex_t escritura;
extern int contador;

#endif

//--------------------------------------------------------------------------------------------------------

/*
ENTRADA: numero_celdas(int) -> cantidad total de celdas del material.
         celda_impactada(int) -> numero de celda del material que impacto la particula.
         posicion_celda_actual(int) -> celda i del material.
         energia_potencial(int) -> energia potencial de la particula que impacto.
SALIDA:  energia_depositada(double) - > energia que deposito la particula en la celda i.
DESCRIPCION: Funcion que calcula la energia depositada sobre una celda i del material
             por una particula.
*/
double calculoEnergiaDepositada(float numero_celdas, float celda_impactada,
                                float posicion_celda_actual, float energia_potencial);

//--------------------------------------------------------------------------------------------------------

/*
ENTRADA: celdas(double *) -> arreglo que contiene la energia total de cada celda del material.
         numero_celdas(int) -> cantidad total de celdas del material.
         posicion_impacto(int) -> posicion en donde impacto la particula.
         energia(float) -> energia de la particula que impacto.
SALIDA:  celdas(double *) - > arreglo recibido pero con la suma de la energia de la particula
         que impacto sumada en cada celda.
DESCRIPCION: Funcion que calcula la energia de una particula que impacto para cada una de las
             celdas y sumando esta al arreglo celdas recibido.
*/
double *calculoEnergiaDepositadaCeldas(double *celdas, int numero_celdas, int posicion_impacto,
                                       float energia);

//--------------------------------------------------------------------------------------------------------

/*
ENTRADA: celdas(double *) -> arreglo que contiene la energia total de cada celda del material.
         numero_celdas(int) -> cantidad total de celdas del material.
SALIDA:  posicion + 1(int) -> posicion de la celda con la mayor energia dentro del arreglo.
DESCRIPCION: Funcion que busca la celda con la mayor energia dentro del arrelo del celdas.
*/
int celdaMayorEnergia(double *celdas, int numero_celdas);

//--------------------------------------------------------------------------------------------------------

/*
ENTRADA: celdas(double *) -> arreglo que contiene la energia total de cada celda del material.
         numero_celdas(int) -> cantidad total de celdas del material.
         celda_mayor_energia(int) -> posicion de la celda con mayor energia dentro del arreglo.
         id_hebras(pthread_t *) -> arreglo con los ids de las hebras.
         lineas_leidas(int *) -> arreglo con la cantidad de lineas leidas por cada hebra.
         numero_hebras(int) -> cantidad total de hebras.
SALIDA:  vacio(void).
DESCRIPCION: Funcion que imprime un grafico normalizado del las energias de cada celda.
*/
void imprimirGrafico(double *celdas, int numero_celdas, int celda_mayor_energia, pthread_t *id_hebras,
                     int *lineas_leidas, int numero_hebras);

//--------------------------------------------------------------------------------------------------------

/*
ENTRADA: args(void *) -> Un puntero genérico a una estructura de datos que contiene los argumentos
                         necesarios para la función lecturaArchivo. En este caso, se espera que
                         sea un puntero a la estructura DataHebra que almacena información compartida
                         entre hebras.
SALIDA:  vacio(void).
DESCRIPCION: Funcion que lee el archivo de entrada utilizando hebras y modificando estructuras de
datos compartidas.
*/
void *lecturaArchivo(void *arg);

//--------------------------------------------------------------------------------------------------------

/*
ENTRADA: nombre_archivo_salida(char *) -> cadena que posee el nombre del archivo a escribir.
         celdas(double *) -> arreglo que contiene la energia total de cada celda del material.
         celda_mayor_energia(int) -> posicion de la celda con mayor energia dentro del arreglo.
         numero_celdas(int) -> cantidad total de celdas del material.
SALIDA:  vacio(void).
DESCRIPCION: Funcion que escribe el archivo de salida con los datos necesarios.
*/
void escrituraArchivoSalida(char *nombre_archivo_salida, double *celdas, int celda_mayor_energia, int numero_celdas);