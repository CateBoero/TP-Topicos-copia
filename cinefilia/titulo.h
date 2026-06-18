#ifndef TITULO_H_INCLUDED
#define TITULO_H_INCLUDED

#include "tipos.h"
#include "indice.h"
#include "funciones.h"

int titulo_validar(const t_titulo *t);
int titulos_cargar_csv(const char *path, t_titulo *arr, int *cant, t_indice *idx, t_incidencias_titulos *inc);
int titulos_cargar_csv_fechado(const char *path, t_titulo *arr, int *cant, t_indice *idx);
int titulos_guardar_csv(const char *path, const t_titulo *arr, int cant);
int titulo_proximo_id(const t_titulo *arr, int cant);
void titulo_alta(t_titulo *arr, int *cant, t_indice *idx);
void titulo_baja(t_titulo *arr, t_indice *idx);
void titulo_modificar(t_titulo *arr, t_indice *idx);
void titulo_mostrar(const t_titulo *t);
void titulo_mostrar_por_id(const t_titulo *arr, const t_indice *idx);
void titulos_listar_sin_stock(const t_titulo *arr, int cant);
void titulos_listar_por_genero(const t_titulo *arr, int cant, const char *genero);
void titulos_listar_stock_bajo(const t_titulo *arr, int cant, int n);

#endif // TITULO_H_INCLUDED
