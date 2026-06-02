#ifndef MIEMBRO_H_INCLUDED
#define MIEMBRO_H_INCLUDED

/* ============================================================
   miembro.h - Operaciones sobre miembros del videoclub
   ============================================================ */

#include "tipos.h"
#include "fecha.h"
#include "indice.h"
#include "funciones.h"

/* Valida un miembro. Retorna ERR_MBR_* o -1 si OK. */
int miembro_validar(const t_miembro *m, t_fecha fp);

/* Carga desde CSV original (miembros.csv).
   Retorna cantidad cargados correctamente. */
int miembros_cargar_csv(const char *path, t_miembro *arr, int *cant,
                        t_indice *idx, t_incidencias_miembros *inc, t_fecha fp);

/* Carga desde CSV fechado generado por el sistema (todos los campos).
   No valida, restaura el estado exacto guardado. */
int miembros_cargar_csv_fechado(const char *path, t_miembro *arr, int *cant,
                                t_indice *idx);

/* Guarda todos los miembros (activos y bajas) en CSV fechado.
   Formato: DNI;CUIL;Nombres;FechaNac;Sexo;FechaAfil;Categoria;FechaUltCuota;Estado;Plan;EmailTutor */
int miembros_guardar_csv(const char *path, const t_miembro *arr, int cant);

/* Alta interactiva de miembro. */
void miembro_alta(t_miembro *arr, int *cant, t_indice *idx, t_fecha fp);

/* Baja logica de miembro por DNI. */
void miembro_baja(t_miembro *arr, t_indice *idx, t_fecha fp);

/* Modificacion de miembro por DNI. */
void miembro_modificar(t_miembro *arr, t_indice *idx, t_fecha fp);

/* Muestra los datos de un miembro. */
void miembro_mostrar(const t_miembro *m);

/* Muestra informacion de un miembro por DNI. */
void miembro_mostrar_por_dni(const t_miembro *arr, const t_indice *idx);

/* Listado de miembros activos ordenado por DNI (via indice). */
void miembros_listar_por_dni(const t_miembro *arr, const t_indice *idx);

/* Listado de miembros activos por plan en formato tabla. */
void miembros_listar_por_plan(const t_miembro *arr, int cant);

#endif /* MIEMBRO_H_INCLUDED */
