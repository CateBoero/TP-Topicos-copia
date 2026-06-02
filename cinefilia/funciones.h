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


int archivos_fechados_existen(t_fecha fp, const char *data_path);

void imprimir_menu(void);

void ver_incidencias(t_fecha fp, t_incidencias_miembros *inc_mbr, t_incidencias_titulos *inc_tit);

#endif // FUNCIONES_H_INCLUDED
