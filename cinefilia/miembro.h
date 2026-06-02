#ifndef MIEMBRO_H_INCLUDED
#define MIEMBRO_H_INCLUDED

#include "tipos.h"
#include "fecha.h"
#include "indice.h"
#include "funciones.h"

int miembro_validar(const t_miembro *m, t_fecha fp);
int miembros_cargar_csv(const char *path, t_miembro *arr, int *cant, t_indice *idx, t_incidencias_miembros *inc, t_fecha fp);

int miembros_cargar_csv_fechado(const char *path, t_miembro *arr, int *cant, t_indice *idx);

/* Guarda todos los miembros (activos y bajas) en CSV fechado.
   Formato: DNI;CUIL;Nombres;FechaNac;Sexo;FechaAfil;Categoria;FechaUltCuota;Estado;Plan;EmailTutor */
int miembros_guardar_csv(const char *path, const t_miembro *arr, int cant);
void miembro_alta(t_miembro *arr, int *cant, t_indice *idx, t_fecha fp);
void miembro_baja(t_miembro *arr, t_indice *idx, t_fecha fp);
void miembro_modificar(t_miembro *arr, t_indice *idx, t_fecha fp);
void miembro_mostrar(const t_miembro *m);
void miembro_mostrar_por_dni(const t_miembro *arr, const t_indice *idx);
void miembros_listar_por_dni(const t_miembro *arr, const t_indice *idx);
void miembros_listar_por_plan(const t_miembro *arr, int cant);

#endif // MIEMBRO_H_INCLUDED
