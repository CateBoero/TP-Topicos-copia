#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#include "tipos.h"
#include "indice.h"

#define ERR_MBR_DNI         0
#define ERR_MBR_NOMBRE      1
#define ERR_MBR_FECHA_NAC   2
#define ERR_MBR_SEXO        3
#define ERR_MBR_FECHA_AFIL  4
#define ERR_MBR_FECHA_CUOTA 5
#define ERR_MBR_PLAN        6
#define ERR_MBR_EMAIL       7
#define ERR_MBR_MAX         8

#define ERR_TIT_ID     0
#define ERR_TIT_TITULO 1
#define ERR_TIT_GENERO 2
#define ERR_TIT_STOCK  3
#define ERR_TIT_MAX    4

#define MAX_INCIDENCIAS 300
#define LIMITE_BASIC      2


typedef struct {
    char     tipo[25];
    unsigned cantidad;
    long     dnis[MAX_INCIDENCIAS];
} t_fila_inc_miembro;

typedef struct {
    char     tipo[25];
    unsigned cantidad;
    int      ids[MAX_INCIDENCIAS];
} t_fila_inc_titulo;

typedef struct {
    t_fila_inc_miembro filas[ERR_MBR_MAX];
} t_incidencias_miembros;

typedef struct {
    t_fila_inc_titulo filas[ERR_TIT_MAX];
} t_incidencias_titulos;


int cmp_reg_indice(const void *a, const void *b);
void calcular_cuil(long dni, char sexo, char *cuil_str);
void normalizar_nombre(const char *src, char *dest);

/* Ordenamiento generico por inserccion, igual criterio que indice_insertar
   pero aplicado in-place sobre un arreglo cualquiera (no usa qsort). */
void ordenamiento_generico(void *base, size_t nmemb, size_t tamanyo,
                           int (*cmp)(const void *, const void *));

/* Compara dos strings ignorando mayusculas/minusculas. Retorna 1 si son iguales. */
int igual_sin_mayus(const char *a, const char *b);

int validar_email(const char *email);
int validar_generico(const void *dato, int (*validar)(const void *));
int validar_dni(const void *dato);
int validar_nombre(const void *dato);
int validar_sexo(const void *dato);
int validar_plan(const void *dato);
int validar_genero(const void *dato);
int validar_stock(const void *dato);
int validar_id_pelicula(const void *dato);


void incidencias_miembros_init(t_incidencias_miembros *inc);
void incidencias_titulos_init(t_incidencias_titulos *inc);
void incidencias_miembros_agregar(t_incidencias_miembros *inc, int tipo, long dni);
void incidencias_titulos_agregar(t_incidencias_titulos *inc, int tipo, int id);
void incidencias_miembros_imprimir(const t_incidencias_miembros *inc);
void incidencias_titulos_imprimir(const t_incidencias_titulos *inc);
void incidencias_ordenar_miembros(t_incidencias_miembros *inc);
void incidencias_ordenar_titulos(t_incidencias_titulos *inc);
int incidencias_miembros_guardar_csv(const t_incidencias_miembros *inc, const char *path);
int incidencias_titulos_guardar_csv(const t_incidencias_titulos *inc, const char *path);
int incidencias_miembros_cargar_csv(t_incidencias_miembros *inc, const char *path);
int incidencias_titulos_cargar_csv(t_incidencias_titulos *inc, const char *path);


int alquiler_buscar(const t_alquiler *arr, int cant, long dni, int id_pelicula);
int alquiler_activos_miembro(const t_alquiler *arr, int cant, long dni);
int alquiler_registrar(t_alquiler *arr, int *cant, t_miembro  *arr_mbr, const t_indice *idx_mbr, t_titulo   *arr_tit, const t_indice *idx_tit);
int alquiler_devolver(t_alquiler *arr, int cant, t_titulo   *arr_tit, const t_indice *idx_tit);
int alquileres_guardar_csv(const char *path, const t_alquiler *arr, int cant);
int alquileres_cargar_csv(const char *path, t_alquiler *arr, int *cant);
void alquileres_listar_indice(const t_alquiler *arr, int cant,
                              const t_miembro *arr_mbr, const t_indice *idx_mbr,
                              const t_titulo *arr_tit, const t_indice *idx_tit);
void miembros_listar_con_alquileres_activos(const t_alquiler *arr_alq, int cant_alq,
                                            const t_miembro *arr_mbr, const t_indice *idx_mbr);
void titulos_listar_mas_alquilados(const t_alquiler *arr_alq, int cant_alq,
                                   const t_titulo *arr_tit, const t_indice *idx_tit);


/* === Funcionalidades extra de defensa === */

/* 1. Miembros que cumplen anios en un mes dado (1-12). */
void miembros_listar_cumpleanios_mes(const t_miembro *arr, int cant, int mes);

/* 2. Miembros sin alquileres historicos. */
void miembros_listar_sin_alquileres(const t_miembro *arr_mbr, int cant_mbr,
                                    const t_alquiler *arr_alq, int cant_alq);

/* 3. Promedio de edad por plan (BASIC/PREMIUM/VIP/FAMILY). */
void miembros_promedio_edad_por_plan(const t_miembro *arr, int cant, t_fecha fp);

/* 4. Titulos que nunca fueron alquilados. */
void titulos_listar_sin_alquileres(const t_titulo *arr_tit, int cant_tit,
                                   const t_alquiler *arr_alq, int cant_alq);

/* 5. Titulos con mayor y menor stock (saltea estado 'B'). */
void titulos_extremos_stock(const t_titulo *arr, int cant);

/* 6. Miembro con mas alquileres historicos totales. */
void miembro_top_historico(const t_alquiler *arr_alq, int cant_alq,
                           const t_miembro *arr_mbr, const t_indice *idx_mbr);

/* 7. Miembros morosos que ademas tienen alquileres sin devolver. */
void miembros_morosos_con_alquileres_pendientes(const t_alquiler *arr_alq, int cant_alq,
                                                const t_miembro *arr_mbr, int cant_mbr,
                                                t_fecha fp);

/* 8. Recaudacion estimada mensual por plan (cuota fija por plan). */
void miembros_recaudacion_por_plan(const t_miembro *arr, int cant);

/* 9. Busqueda por rango sobre el indice de miembros (lower bound + barrido). */
void miembros_buscar_por_rango_dni(const t_miembro *arr, const t_indice *idx,
                                   long dni_min, long dni_max);

/* 10. Exporta a CSV los miembros activos (DNI;Nombre;Plan;Cuota). */
int miembros_exportar_activos_csv(const t_miembro *arr, int cant, const char *path);

void menu_extra_defensa(const t_miembro *arr_mbr, int cant_mbr, const t_indice *idx_mbr,
                        const t_titulo *arr_tit, int cant_tit,
                        const t_alquiler *arr_alq, int cant_alq, t_fecha fp);


int archivos_fechados_existen(t_fecha fp, const char *data_path);

void imprimir_menu(void);

void ver_incidencias(t_fecha fp, t_incidencias_miembros *inc_mbr, t_incidencias_titulos *inc_tit);

#endif // FUNCIONES_H_INCLUDED
