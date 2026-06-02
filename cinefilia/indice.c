#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "indice.h"

void indice_crear(t_indice *indice, size_t nmemb, size_t tamanyo) {
    indice->vindice = calloc(nmemb, tamanyo);
    if (!indice->vindice) {
        printf("  [ERROR] No se pudo asignar memoria para el indice\n");
        fflush(stdout);
        return;
    }
    
    indice->cantidad_elementos_actual = 0;
    indice->cantidad_elementos_maxima = (unsigned)nmemb;
}

void indice_redimensionar(t_indice *indice, size_t nmemb, size_t tamanyo) {
    void *nuevo = realloc(indice->vindice, nmemb * tamanyo);
    if (nuevo != NULL) {
        indice->vindice                   = nuevo;
        indice->cantidad_elementos_maxima = (unsigned)nmemb;
    }
}

int indice_insertar(t_indice *indice, const void *registro, size_t tamanyo,
                    int (*cmp)(const void *, const void *)) {
    unsigned pos, i;
    char    *base;

    /* Ampliar si esta lleno */
    if (indice_lleno(indice) == OK) {
        unsigned nueva_max = (unsigned)(indice->cantidad_elementos_maxima * INCREMENTO);
        if (nueva_max <= indice->cantidad_elementos_maxima)
            nueva_max = indice->cantidad_elementos_maxima + 1;
        indice_redimensionar(indice, (size_t)nueva_max, tamanyo);
        if (indice_lleno(indice) == OK) return ERROR;
    }

    base = (char *)indice->vindice;

    /* Buscar posicion de insercion ordenada */
    pos = indice->cantidad_elementos_actual;
    for (i = 0; i < indice->cantidad_elementos_actual; i++) {
        if (cmp(registro, base + i * tamanyo) < 0) {
            pos = i;
            break;
        }
    }

    /* Desplazar elementos a la derecha */
    for (i = indice->cantidad_elementos_actual; i > pos; i--)
        memcpy(base + i * tamanyo, base + (i - 1) * tamanyo, tamanyo);

    /* Insertar */
    memcpy(base + pos * tamanyo, registro, tamanyo);
    indice->cantidad_elementos_actual++;
    return OK;
}

int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo,
                    int (*cmp)(const void *, const void *)) {
    int      pos;
    unsigned i;
    char    *base;

    pos = indice_buscar(indice, registro,
                        indice->cantidad_elementos_actual, tamanyo, cmp);
    if (pos == NO_EXISTE) return ERROR;

    base = (char *)indice->vindice;

    /* Desplazar elementos a la izquierda */
    for (i = (unsigned)pos; i < indice->cantidad_elementos_actual - 1; i++)
        memcpy(base + i * tamanyo, base + (i + 1) * tamanyo, tamanyo);

    indice->cantidad_elementos_actual--;
    return OK;
}

int indice_buscar(const t_indice *indice, const void *registro, size_t nmemb,
                  size_t tamanyo, int (*cmp)(const void *, const void *)) {
    int         izq = 0, der = (int)nmemb - 1, mid, res;
    const char *base = (const char *)indice->vindice;

    while (izq <= der) {
        mid = (izq + der) / 2;
        res = cmp(registro, base + (size_t)mid * tamanyo);
        if (res == 0) return mid;
        if (res  < 0) der = mid - 1;
        else          izq = mid + 1;
    }
    return NO_EXISTE;
}

int indice_vacio(const t_indice *indice) {
    return (indice->cantidad_elementos_actual == 0) ? OK : 0;
}

int indice_lleno(const t_indice *indice) {
    return (indice->cantidad_elementos_actual >= indice->cantidad_elementos_maxima) ? OK : 0;
}

int indice_cargar(const char *path, t_indice *indice, void *vreg_ind,
                  size_t tamanyo, int (*cmp)(const void *, const void *)) {
    FILE *f;

    f = fopen(path, "rb");
    if (!f) return ERROR;

    while (fread(vreg_ind, tamanyo, 1, f) == 1) {
        if (indice_insertar(indice, vreg_ind, tamanyo, cmp) == ERROR) {
            fclose(f);
            return ERROR;
        }
    }

    fclose(f);
    return OK;
}

void indice_vaciar(t_indice *indice) {
    if (indice->vindice) {
        free(indice->vindice);
        indice->vindice = NULL;
    }
    indice->cantidad_elementos_actual = 0;
    indice->cantidad_elementos_maxima = 0;
}
