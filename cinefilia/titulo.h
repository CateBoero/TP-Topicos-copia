#ifndef TITULO_H_INCLUDED
#define TITULO_H_INCLUDED

/* ============================================================
   titulo.h - Operaciones sobre titulos del videoclub
   ============================================================ */

#include "tipos.h"
#include "indice.h"
#include "funciones.h"

/* Valida un titulo. Retorna ERR_TIT_* o -1 si OK. */
int titulo_validar(const t_titulo *t);

/* Carga desde CSV original (titulos.csv).
   Retorna cantidad cargados correctamente. */
int titulos_cargar_csv(const char *path, t_titulo *arr, int *cant,
                       t_indice *idx, t_incidencias_titulos *inc);

/* Carga desde CSV fechado generado por el sistema.
   No valida, restaura el estado exacto guardado. */
int titulos_cargar_csv_fechado(const char *path, t_titulo *arr, int *cant,
                               t_indice *idx);

/* Guarda todos los titulos en CSV fechado.
   Formato: ID;Titulo;Genero;Stock;Estado */
int titulos_guardar_csv(const char *path, const t_titulo *arr, int cant);

/* Retorna el proximo ID disponible (max + 1). */
int titulo_proximo_id(const t_titulo *arr, int cant);

/* Alta interactiva de titulo. */
void titulo_alta(t_titulo *arr, int *cant, t_indice *idx);

/* Baja logica de titulo por ID. */
void titulo_baja(t_titulo *arr, t_indice *idx);

/* Modificacion de titulo por ID. */
void titulo_modificar(t_titulo *arr, t_indice *idx);

/* Muestra los datos de un titulo. */
void titulo_mostrar(const t_titulo *t);

/* Muestra informacion de un titulo por ID. */
void titulo_mostrar_por_id(const t_titulo *arr, const t_indice *idx);

#endif /* TITULO_H_INCLUDED */
