#ifndef FECHA_H_INCLUDED
#define FECHA_H_INCLUDED

#include "tipos.h"

/* Retorna 1 si el anio es bisiesto */
int es_bisiesto(int anio);

/* Retorna la cantidad de dias del mes */
int dias_en_mes(int mes, int anio);

/* Retorna 1 si la fecha es valida */
int fecha_valida(t_fecha f);

/* Retorna la fecha actual del sistema */
t_fecha fecha_actual(void);

/* Retorna 1 si f1 <= f2 */
int fecha_menor_igual(t_fecha f1, t_fecha f2);

/* Retorna 1 si f1 == f2 */
int fecha_igual(t_fecha f1, t_fecha f2);

/* Calcula la edad en anios a la fecha de proceso */
int calcular_edad(t_fecha nacimiento, t_fecha proceso);

/* Parsea fecha desde string DD/MM/YYYY. Retorna 1 si OK */
int fecha_desde_string(const char *str, t_fecha *f);

/* Imprime la fecha como DD/MM/YYYY */
void fecha_imprimir(t_fecha f);

/* Formatea la fecha como string YYYYMMDD para nombres de archivo */
void fecha_a_string_archivo(t_fecha f, char *buf);

/* Solicita la fecha de proceso por teclado.
   Si el usuario presiona Enter, retorna la fecha del sistema. */
t_fecha pedir_fecha_proceso(void);

#endif // FECHA_H_INCLUDED
