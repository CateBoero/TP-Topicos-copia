#ifndef INDICE_H_INCLUDED
#define INDICE_H_INCLUDED

/* ============================================================
   indice.h - TDA Indice con asignacion dinamica de memoria
   Especificacion de la catedra
   ============================================================ */

#include <stddef.h>

#define CANTIDAD_ELEMENTOS 100
#define INCREMENTO         1.3
#define OK                 1
#define ERROR              0
#define NO_EXISTE         -1

typedef struct {
    unsigned nro_reg;
    long     dni;
} t_reg_indice;

typedef struct {
    void    *vindice;
    unsigned cantidad_elementos_actual;
    unsigned cantidad_elementos_maxima;
} t_indice;

/**************************************************************************
Descripcion:  toma memoria para 100 elementos e inicializa la estructura vacia.
Parametros:   indice  : TDA indice.
              nmemb   : cantidad de elementos del indice.
              tamanyo : el espacio en bytes ocupado por cada elemento.
Retorno:      n/a.
Observaciones:
**************************************************************************/
void indice_crear(t_indice *indice, size_t nmemb, size_t tamanyo);

/**************************************************************************
Descripcion:  redimensiona el tamanyo del indice.
Parametros:   indice  : TDA indice.
              nmemb   : cantidad de elementos del indice.
              tamanyo : el espacio en bytes ocupado por cada elemento.
Retorno:      n/a.
Observaciones: Debe proporcionar el nmemb incrementado en un 30%
**************************************************************************/
void indice_redimensionar(t_indice *indice, size_t nmemb, size_t tamanyo);

/**************************************************************************
Descripcion:  inserta en orden segun la clave.
Parametros:   indice   : TDA indice.
              registro : el nuevo elemento a insertar en el indice.
              tamanyo  : el espacio en bytes ocupado por el elemento a insertar.
              cmp      : funcion de comparacion provista.
Retorno:      OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones: Si el array esta lleno, toma un 30% mas de memoria.
**************************************************************************/
int indice_insertar(t_indice *indice, const void *registro, size_t tamanyo,
                    int (*cmp)(const void *, const void *));

/**************************************************************************
Descripcion:  elimina el registro del indice.
Parametros:   indice   : TDA indice.
              registro : el elemento a eliminar.
              tamanyo  : el espacio en bytes ocupado por el elemento a insertar.
              cmp      : funcion de comparacion provista.
Retorno:      OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones: -
**************************************************************************/
int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo,
                    int (*cmp)(const void *, const void *));

/**************************************************************************
Descripcion:  si la clave existe deja el registro en registro.
Parametros:   indice   : TDA indice.
              registro : el elemento a buscar.
              nmemb    : cantidad de elementos del indice.
              tamanyo  : espacio en bytes ocupado por el elemento a insertar.
              cmp      : funcion de comparacion provista.
Retorno:      NO_EXISTE si no existe o si existe, la posicion ocupada dentro
              del array.
Observaciones: -
**************************************************************************/
int indice_buscar(const t_indice *indice, const void *registro, size_t nmemb,
                  size_t tamanyo, int (*cmp)(const void *, const void *));

/**************************************************************************
Descripcion:  determina si el indice contiene 0 (cero) elementos.
Parametros:   indice : TDA indice.
Retorno:      OK si esta vacio, cualquier otro valor si no lo esta.
Observaciones: -
**************************************************************************/
int indice_vacio(const t_indice *indice);

/**************************************************************************
descripcion:  determina si el indice contiene el tamanyo maximo posible.
Parametros:   indice : TDA indice.
Retorno:      OK si esta lleno, cualquier otro valor si no lo esta.
Observaciones: -
**************************************************************************/
int indice_lleno(const t_indice *indice);

/**************************************************************************
Descripcion:  deja el indice vacio.
Parametros:   indice : TDA indice.
Retorno:      No posee.
Observaciones: -
**************************************************************************/
/**************************************************************************
Descripcion:  Carga el array desde un archivo ordenado.
Parametros:   path     : la ruta al archivo binario.
              indice   : TDA indice.
              vreg_ind : vector de elementos dentro del indice.
              tamanyo  : el espacio en bytes ocupado por el elemento a insertar.
              cmp      : funcion de comparacion provista.
Retorno:      OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones: -
**************************************************************************/
int indice_cargar(const char *path, t_indice *indice, void *vreg_ind,
                  size_t tamanyo, int (*cmp)(const void *, const void *));

/**************************************************************************
Descripcion:  libera la memoria del indice y lo deja inutilizable.
Parametros:   indice : TDA indice.
Retorno:      n/a.
Observaciones: -
**************************************************************************/
void indice_vaciar(t_indice *indice);

#endif // INDICE_H_INCLUDED
