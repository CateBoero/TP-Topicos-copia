#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tipos.h"
#include "funciones.h"
#include "fecha.h"

#define MAX_PALABRAS 20
#define MAX_LEN_PAL  65

int cmp_reg_indice(const void *a, const void *b) {
    const t_reg_indice *ra = (const t_reg_indice *)a;
    const t_reg_indice *rb = (const t_reg_indice *)b;
    if (ra->dni < rb->dni) return -1;
    if (ra->dni > rb->dni) return  1;
    return 0;
}

void ordenamiento_generico(void *base, size_t nmemb, size_t tamanyo,
                           int (*cmp)(const void *, const void *)) {
    char  *arr = (char *)base;
    char  *tmp;
    size_t i, j;

    tmp = malloc(tamanyo);
    if (!tmp) return;

    for (i = 1; i < nmemb; i++) {
        memcpy(tmp, arr + i * tamanyo, tamanyo);
        j = i;
        while (j > 0 && cmp(arr + (j - 1) * tamanyo, tmp) > 0) {
            memcpy(arr + j * tamanyo, arr + (j - 1) * tamanyo, tamanyo);
            j--;
        }
        memcpy(arr + j * tamanyo, tmp, tamanyo);
    }

    free(tmp);
}

int igual_sin_mayus(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

void calcular_cuil(long dni, char sexo, char *cuil_str) {
    static const int pesos[] = {5, 4, 3, 2, 7, 6, 5, 4, 3, 2};
    int  digitos[10], xy, z, suma, resto, i;
    long long numero;

    xy     = (sexo == 'F') ? 27 : 20;
    numero = (long long)xy * 100000000L + dni;

    for (i = 9; i >= 0; i--) {
        digitos[i] = (int)(numero % 10);
        numero    /= 10;
    }

    suma = 0;
    for (i = 0; i < 10; i++)
        suma += digitos[i] * pesos[i];

    resto = suma % 11;

    if (resto == 0) {
        z = 0;
    } else if (resto == 1) {
        z  = (sexo == 'F') ? 4 : 9;
        xy = 23;
    } else {
        z = 11 - resto;
    }

    sprintf(cuil_str, "%02d-%08ld-%d", xy, dni, z);
}

void normalizar_nombre(const char *src, char *dest) {
    char palabras[MAX_PALABRAS][MAX_LEN_PAL];
    int  n_palabras = 0, pos_coma = -1, tiene_coma = 0;
    int  i, j, k, len;
    char temp[130];

    len = (int)strlen(src);
    i   = 0;

    while (i < len && n_palabras < MAX_PALABRAS) {
        while (i < len && src[i] == ' ') i++;
        if (i >= len) break;

        if (src[i] == ',') {
            if (!tiene_coma && n_palabras > 0) {
                pos_coma   = n_palabras - 1;
                tiene_coma = 1;
            }
            i++;
            continue;
        }

        j = 0;
        while (i < len && src[i] != ' ' && src[i] != ',') {
            if (j < MAX_LEN_PAL - 1)
                palabras[n_palabras][j++] = src[i];
            i++;
        }
        palabras[n_palabras][j] = '\0';
        if (j > 0) n_palabras++;
    }

    if (!tiene_coma && n_palabras > 1)
        pos_coma = 0;

    k = 0;
    for (i = 0; i < n_palabras && k < 128; i++) {
        for (j = 0; palabras[i][j] != '\0' && k < 128; j++)
            temp[k++] = (j == 0) ? (char)toupper((unsigned char)palabras[i][j])
                                 : (char)tolower((unsigned char)palabras[i][j]);

        if (i < n_palabras - 1 && k < 127) {
            if (i == pos_coma) { temp[k++] = ','; temp[k++] = ' '; }
            else                temp[k++] = ' ';
        }
    }
    temp[k] = '\0';
    strncpy(dest, temp, LEN_NOMBRE - 1);
    dest[LEN_NOMBRE - 1] = '\0';
}

int validar_email(const char *email) {
    int i, len, arroba_pos, domain_start, punto_dom;

    if (!email || email[0] == '\0') return 0;
    len = (int)strlen(email);

    for (i = 0; i < len; i++)
        if (email[i] == ' ') return 0;

    arroba_pos = -1;
    for (i = 0; i < len; i++) {
        if (email[i] == '@') {
            if (arroba_pos != -1) return 0;
            arroba_pos = i;
        }
    }
    if (arroba_pos < 1) return 0;

    for (i = 0; i < arroba_pos; i++)
        if (!isalnum((unsigned char)email[i]) &&
            email[i] != '.' && email[i] != '_' && email[i] != '-')
            return 0;

    domain_start = arroba_pos + 1;
    if (domain_start >= len) return 0;

    punto_dom = -1;
    for (i = domain_start; i < len; i++) {
        if (email[i] == '.') punto_dom = i;
        else if (!isalnum((unsigned char)email[i]) && email[i] != '-')
            return 0;
    }
    if (punto_dom <= domain_start || punto_dom == len - 1) return 0;

    return 1;
}

int validar_generico(const void *dato, int (*validar)(const void *)) {
    return validar(dato);
}

int validar_dni(const void *dato) {
    long dni = *(const long *)dato;
    return (dni > 1000000L && dni < 100000000L);
}

int validar_nombre(const void *dato) {
    const char *nombre = (const char *)dato;
    int i;
    if (!nombre || nombre[0] == '\0') return 0;
    for (i = 0; nombre[i] != '\0'; i++)
        if (isalpha((unsigned char)nombre[i])) return 1;
    return 0;
}

int validar_sexo(const void *dato) {
    char s = *(const char *)dato;
    return (s == 'F' || s == 'M' || s == 'O');
}

int validar_plan(const void *dato) {
    const char *p = (const char *)dato;
    return (strcmp(p, "BASIC")   == 0 || strcmp(p, "PREMIUM") == 0 ||
            strcmp(p, "VIP")     == 0 || strcmp(p, "FAMILY")  == 0);
}

int validar_genero(const void *dato) {
    const char *g = (const char *)dato;
    return (strcmp(g, "Accion")  == 0 || strcmp(g, "Drama")   == 0 ||
            strcmp(g, "Comedia") == 0 || strcmp(g, "Terror")  == 0);
}

int validar_stock(const void *dato) {
    int stock = *(const int *)dato;
    return (stock >= 0);
}

int validar_id_pelicula(const void *dato) {
    int id = *(const int *)dato;
    return (id > 0);
}

static const char *nombres_err_mbr[ERR_MBR_MAX] = {
    "DNI", "NOMBRE", "FECHA_NAC", "SEXO",
    "FECHA_AFIL", "FECHA_CUOTA", "PLAN", "EMAIL"
};

static const char *nombres_err_tit[ERR_TIT_MAX] = {
    "ID", "TITULO", "GENERO", "STOCK"
};

void incidencias_miembros_init(t_incidencias_miembros *inc) {
    int i;

    memset(inc, 0, sizeof(t_incidencias_miembros));

    for (i = 0; i < ERR_MBR_MAX; i++) {
        if (nombres_err_mbr[i] != NULL) {
            strncpy(inc->filas[i].tipo, nombres_err_mbr[i], 24);
            inc->filas[i].tipo[24] = '\0';
        }
        inc->filas[i].cantidad = 0;
    }
}

void incidencias_titulos_init(t_incidencias_titulos *inc) {
    int i;

    memset(inc, 0, sizeof(t_incidencias_titulos));

    for (i = 0; i < ERR_TIT_MAX; i++) {
        if (nombres_err_tit[i] != NULL) {
            strncpy(inc->filas[i].tipo, nombres_err_tit[i], 24);
            inc->filas[i].tipo[24] = '\0';
        }
        inc->filas[i].cantidad = 0;
    }
}

void incidencias_miembros_agregar(t_incidencias_miembros *inc, int tipo, long dni) {
    if (tipo < 0 || tipo >= ERR_MBR_MAX) return;
    if (inc->filas[tipo].cantidad < MAX_INCIDENCIAS) {
        inc->filas[tipo].dnis[inc->filas[tipo].cantidad] = dni;
        inc->filas[tipo].cantidad++;
    }
}

void incidencias_titulos_agregar(t_incidencias_titulos *inc, int tipo, int id) {
    if (tipo < 0 || tipo >= ERR_TIT_MAX) return;
    if (inc->filas[tipo].cantidad < MAX_INCIDENCIAS) {
        inc->filas[tipo].ids[inc->filas[tipo].cantidad] = id;
        inc->filas[tipo].cantidad++;
    }
}

void incidencias_miembros_imprimir(const t_incidencias_miembros *inc) {
    int i, j, hay = 0;
    printf("\n=== INCIDENCIAS - MIEMBROS ===\n");
    printf("%-15s | %-10s | IDs afectados\n", "Tipo Error", "Cantidad");
    printf("-------------------------------------------------------\n");
    for (i = 0; i < ERR_MBR_MAX; i++) {
        if (inc->filas[i].cantidad > 0) {
            hay = 1;
            printf("%-15s | %-10u | ", inc->filas[i].tipo, inc->filas[i].cantidad);
            for (j = 0; j < (int)inc->filas[i].cantidad; j++) {
                if (j > 0) printf(", ");
                printf("%ld", inc->filas[i].dnis[j]);
            }
            printf("\n");
        }
    }
    if (!hay) printf("Sin incidencias de validacion en miembros.\n");
}

void incidencias_titulos_imprimir(const t_incidencias_titulos *inc) {
    int i, j, hay = 0;
    printf("\n=== INCIDENCIAS - TITULOS ===\n");
    printf("%-10s | %-10s | IDs afectados\n", "Tipo Error", "Cantidad");
    printf("---------------------------------------------------\n");
    for (i = 0; i < ERR_TIT_MAX; i++) {
        if (inc->filas[i].cantidad > 0) {
            hay = 1;
            printf("%-10s | %-10u | ", inc->filas[i].tipo, inc->filas[i].cantidad);
            for (j = 0; j < (int)inc->filas[i].cantidad; j++) {
                if (j > 0) printf(", ");
                printf("%d", inc->filas[i].ids[j]);
            }
            printf("\n");
        }
    }
    if (!hay) printf("Sin incidencias de validacion en titulos.\n");
}

static int cmp_fila_mbr(const void *a, const void *b) {
    const t_fila_inc_miembro *fa = (const t_fila_inc_miembro *)a;
    const t_fila_inc_miembro *fb = (const t_fila_inc_miembro *)b;
    if (fb->cantidad > fa->cantidad) return  1;
    if (fb->cantidad < fa->cantidad) return -1;
    return 0;
}

static int cmp_fila_tit(const void *a, const void *b) {
    const t_fila_inc_titulo *fa = (const t_fila_inc_titulo *)a;
    const t_fila_inc_titulo *fb = (const t_fila_inc_titulo *)b;
    if (fb->cantidad > fa->cantidad) return  1;
    if (fb->cantidad < fa->cantidad) return -1;
    return 0;
}

void incidencias_ordenar_miembros(t_incidencias_miembros *inc) {
    qsort(inc->filas, ERR_MBR_MAX, sizeof(t_fila_inc_miembro), cmp_fila_mbr);
}

void incidencias_ordenar_titulos(t_incidencias_titulos *inc) {
    qsort(inc->filas, ERR_TIT_MAX, sizeof(t_fila_inc_titulo), cmp_fila_tit);
}

int incidencias_miembros_guardar_csv(const t_incidencias_miembros *inc, const char *path) {
    FILE *f;
    int   i, j;

    f = fopen(path, "w");
    if (!f) return 0;

    for (i = 0; i < ERR_MBR_MAX; i++) {
        fprintf(f, "%s;%u", inc->filas[i].tipo, inc->filas[i].cantidad);
        for (j = 0; j < (int)inc->filas[i].cantidad; j++)
            fprintf(f, ";%ld", inc->filas[i].dnis[j]);
        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

int incidencias_titulos_guardar_csv(const t_incidencias_titulos *inc, const char *path) {
    FILE *f;
    int   i, j;

    f = fopen(path, "w");
    if (!f) return 0;

    for (i = 0; i < ERR_TIT_MAX; i++) {
        fprintf(f, "%s;%u", inc->filas[i].tipo, inc->filas[i].cantidad);
        for (j = 0; j < (int)inc->filas[i].cantidad; j++)
            fprintf(f, ";%d", inc->filas[i].ids[j]);
        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

static int csv_split_buf(char *linea, char **campos, int max) {
    int   n   = 0;
    char *ptr = linea;
    if (!linea || !linea[0]) return 0;
    campos[n++] = ptr;
    while (*ptr && n < max) {
        if (*ptr == ';') { *ptr = '\0'; campos[n++] = ptr + 1; }
        ptr++;
    }
    ptr = campos[n - 1] + strlen(campos[n - 1]) - 1;
    while (ptr >= campos[n - 1] && (*ptr == '\n')) *ptr-- = '\0';
    return n;
}

int incidencias_miembros_cargar_csv(t_incidencias_miembros *inc, const char *path) {
    FILE  *f;
    char   linea[4096];
    char  *campos[MAX_INCIDENCIAS + 3];
    int    n, i, j, fila_idx;

    f = fopen(path, "r");
    if (!f) return 0;

    incidencias_miembros_init(inc);

    while (fgets(linea, sizeof(linea), f)) {
        if (linea[0] == '\n' || linea[0] == '#') continue;
        n = csv_split_buf(linea, campos, MAX_INCIDENCIAS + 3);
        if (n < 2) continue;

        fila_idx = -1;
        for (i = 0; i < ERR_MBR_MAX; i++) {
            if (strcmp(inc->filas[i].tipo, campos[0]) == 0) {
                fila_idx = i; break;
            }
        }
        if (fila_idx == -1) continue;

        inc->filas[fila_idx].cantidad = (unsigned)atoi(campos[1]);
        for (j = 2; j < n && (j - 2) < (int)inc->filas[fila_idx].cantidad; j++)
            inc->filas[fila_idx].dnis[j - 2] = atol(campos[j]);
    }

    fclose(f);
    return 1;
}

int incidencias_titulos_cargar_csv(t_incidencias_titulos *inc, const char *path) {
    FILE  *f;
    char   linea[4096];
    char  *campos[MAX_INCIDENCIAS + 3];
    int    n, i, j, fila_idx;

    f = fopen(path, "r");
    if (!f) return 0;

    incidencias_titulos_init(inc);

    while (fgets(linea, sizeof(linea), f)) {
        if (linea[0] == '\n'|| linea[0] == '#') continue;
        n = csv_split_buf(linea, campos, MAX_INCIDENCIAS + 3);
        if (n < 2) continue;

        fila_idx = -1;
        for (i = 0; i < ERR_TIT_MAX; i++) {
            if (strcmp(inc->filas[i].tipo, campos[0]) == 0) {
                fila_idx = i; break;
            }
        }
        if (fila_idx == -1) continue;

        inc->filas[fila_idx].cantidad = (unsigned)atoi(campos[1]);
        for (j = 2; j < n && (j - 2) < (int)inc->filas[fila_idx].cantidad; j++)
            inc->filas[fila_idx].ids[j - 2] = atoi(campos[j]);
    }

    fclose(f);
    return 1;
}

int alquiler_buscar(const t_alquiler *arr, int cant, long dni, int id_pelicula) {
    int i;
    for (i = 0; i < cant; i++)
        if (arr[i].dni == dni && arr[i].id_pelicula == id_pelicula)
            return i;
    return -1;
}

int alquiler_activos_miembro(const t_alquiler *arr, int cant, long dni) {
    int i, total = 0;
    for (i = 0; i < cant; i++)
        if (arr[i].dni == dni)
            total += arr[i].alquileres_activos;
    return total;
}

static void leer_linea_alq(char *buf, int max) {
    if (fgets(buf, max, stdin)) {
        int len = (int)strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
    }
}

int alquiler_registrar(t_alquiler *arr, int *cant,
                       t_miembro  *arr_mbr, const t_indice *idx_mbr,
                       t_titulo   *arr_tit, const t_indice *idx_tit) {
    char         buf[64];
    long         dni;
    int          id_pelicula;
    t_reg_indice ri;
    int          pos_mbr, pos_tit, pos_alq;
    t_miembro   *mbr;
    t_titulo    *tit;

    printf("DNI del miembro: ");
    leer_linea_alq(buf, sizeof(buf));
    dni = atol(buf);

    ri.dni     = dni;
    ri.nro_reg = 0;
    pos_mbr = indice_buscar(idx_mbr, &ri, idx_mbr->cantidad_elementos_actual,
                            sizeof(t_reg_indice), cmp_reg_indice);
    if (pos_mbr == NO_EXISTE) {
        printf("Miembro con DNI %ld no encontrado o dado de baja.\n", dni);
        return 0;
    }
    ri  = ((t_reg_indice *)idx_mbr->vindice)[pos_mbr];
    mbr = &arr_mbr[ri.nro_reg];

    if (strcmp(mbr->plan, "BASIC") == 0 &&
        alquiler_activos_miembro(arr, *cant, dni) >= LIMITE_BASIC) {
        printf("El miembro BASIC ya tiene %d peliculas alquiladas (maximo %d).\n",
               LIMITE_BASIC, LIMITE_BASIC);
        return 0;
    }

    printf("ID de la pelicula: ");
    leer_linea_alq(buf, sizeof(buf));
    id_pelicula = atoi(buf);

    ri.dni     = (long)id_pelicula;
    ri.nro_reg = 0;
    pos_tit = indice_buscar(idx_tit, &ri, idx_tit->cantidad_elementos_actual,
                            sizeof(t_reg_indice), cmp_reg_indice);
    if (pos_tit == NO_EXISTE) {
        printf("Titulo con ID %d no encontrado.\n", id_pelicula); return 0;
    }
    ri  = ((t_reg_indice *)idx_tit->vindice)[pos_tit];
    tit = &arr_tit[ri.nro_reg];

    if (tit->estado != 'A') { printf("La pelicula esta dada de baja.\n"); return 0; }
    if (tit->stock  <= 0)   { printf("Sin stock disponible.\n"); return 0; }

    tit->stock--;

    pos_alq = alquiler_buscar(arr, *cant, dni, id_pelicula);
    if (pos_alq != -1) {
        arr[pos_alq].total_alquileres++;
        arr[pos_alq].alquileres_activos++;
    } else {
        if (*cant >= MAX_ALQUILERES) { printf("Capacidad maxima alcanzada.\n"); return 0; }
        arr[*cant].dni                = dni;
        arr[*cant].id_pelicula        = id_pelicula;
        arr[*cant].total_alquileres   = 1;
        arr[*cant].alquileres_activos = 1;
        (*cant)++;
    }

    printf("Alquiler registrado: '%s' para DNI %ld. Stock restante: %d\n",
           tit->titulo, mbr->dni, tit->stock);
    return 1;
}

int alquiler_devolver(t_alquiler *arr, int cant,
                      t_titulo   *arr_tit, const t_indice *idx_tit) {
    char         buf[64];
    long         dni;
    int          id_pelicula, pos_alq, pos_tit;
    t_reg_indice ri;

    printf("DNI del miembro: ");
    leer_linea_alq(buf, sizeof(buf));
    dni = atol(buf);

    printf("ID de la pelicula a devolver: ");
    leer_linea_alq(buf, sizeof(buf));
    id_pelicula = atoi(buf);

    pos_alq = alquiler_buscar(arr, cant, dni, id_pelicula);
    if (pos_alq == -1 || arr[pos_alq].alquileres_activos <= 0) {
        printf("No existe alquiler activo para ese miembro y pelicula.\n");
        return 0;
    }

    arr[pos_alq].alquileres_activos--;

    ri.dni     = (long)id_pelicula;
    ri.nro_reg = 0;
    pos_tit = indice_buscar(idx_tit, &ri, idx_tit->cantidad_elementos_actual,
                            sizeof(t_reg_indice), cmp_reg_indice);
    if (pos_tit != NO_EXISTE) {
        ri = ((t_reg_indice *)idx_tit->vindice)[pos_tit];
        arr_tit[ri.nro_reg].stock++;
    }

    printf("Devolucion registrada. Alquileres historicos: %d\n",
           arr[pos_alq].total_alquileres);
    return 1;
}

int alquileres_guardar_csv(const char *path, const t_alquiler *arr, int cant) {
    FILE *f;
    int   i;

    f = fopen(path, "w");
    if (!f) return 0;

    for (i = 0; i < cant; i++)
        fprintf(f, "%ld;%d;%d;%d\n",
                arr[i].dni, arr[i].id_pelicula,
                arr[i].total_alquileres, arr[i].alquileres_activos);

    fclose(f);
    return 1;
}

int alquileres_cargar_csv(const char *path, t_alquiler *arr, int *cant) {
    FILE  *f;
    char   linea[128];
    char  *campos[5];
    int    n;

    f = fopen(path, "r");
    if (!f) return 0;

    *cant = 0;
    while (fgets(linea, sizeof(linea), f) && *cant < MAX_ALQUILERES) {
        if (linea[0] == '\n' || linea[0] == '#') continue;
        n = csv_split_buf(linea, campos, 5);
        if (n < 4) continue;
        arr[*cant].dni                = atol(campos[0]);
        arr[*cant].id_pelicula        = atoi(campos[1]);
        arr[*cant].total_alquileres   = atoi(campos[2]);
        arr[*cant].alquileres_activos = atoi(campos[3]);
        (*cant)++;
    }

    fclose(f);
    return *cant;
}

void alquileres_listar_indice(const t_alquiler *arr, int cant,
                              const t_miembro *arr_mbr, const t_indice *idx_mbr,
                              const t_titulo *arr_tit, const t_indice *idx_tit) {
    t_indice      idx_alq;
    t_reg_indice  clave, *ri;
    unsigned      i;
    int           pos;

    if (cant == 0) { printf("No hay alquileres registrados.\n"); return; }

    /* Indice transitorio de alquileres, ordenado por DNI, siguiendo el
       mismo TDA usado para indice_miembros / indice_titulos. */
    indice_crear(&idx_alq, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));
    for (i = 0; i < (unsigned)cant; i++) {
        clave.dni     = arr[i].dni;
        clave.nro_reg = i;
        indice_insertar(&idx_alq, &clave, sizeof(t_reg_indice), cmp_reg_indice);
    }

    printf("\n%-12s %-30s %-30s %-8s %-8s\n",
           "DNI", "Miembro", "Titulo", "Totales", "Activos");
    printf("%-12s %-30s %-30s %-8s %-8s\n",
           "------------", "------------------------------",
           "------------------------------", "--------", "--------");

    for (i = 0; i < idx_alq.cantidad_elementos_actual; i++) {
        const t_alquiler *a;
        const char       *nombre = "(desconocido)";
        const char       *titulo = "(desconocido)";

        ri = (t_reg_indice *)idx_alq.vindice + i;
        a  = &arr[ri->nro_reg];

        clave.dni = a->dni;
        pos = indice_buscar(idx_mbr, &clave, idx_mbr->cantidad_elementos_actual,
                            sizeof(t_reg_indice), cmp_reg_indice);
        if (pos != NO_EXISTE) {
            unsigned nro = ((t_reg_indice *)idx_mbr->vindice)[pos].nro_reg;
            nombre = arr_mbr[nro].apellidos_nombres;
        }

        clave.dni = (long)a->id_pelicula;
        pos = indice_buscar(idx_tit, &clave, idx_tit->cantidad_elementos_actual,
                            sizeof(t_reg_indice), cmp_reg_indice);
        if (pos != NO_EXISTE) {
            unsigned nro = ((t_reg_indice *)idx_tit->vindice)[pos].nro_reg;
            titulo = arr_tit[nro].titulo;
        }

        printf("%-12ld %-30s %-30s %-8d %-8d\n",
               a->dni, nombre, titulo, a->total_alquileres, a->alquileres_activos);
    }

    indice_vaciar(&idx_alq);
}

typedef struct { long dni;         int total; } t_acc_miembro;
typedef struct { int  id_pelicula; int total; } t_acc_titulo;

static int cmp_acc_miembro_desc(const void *a, const void *b) {
    return ((const t_acc_miembro *)b)->total - ((const t_acc_miembro *)a)->total;
}

static int cmp_acc_titulo_desc(const void *a, const void *b) {
    return ((const t_acc_titulo *)b)->total - ((const t_acc_titulo *)a)->total;
}

void miembros_listar_con_alquileres_activos(const t_alquiler *arr_alq, int cant_alq,
                                            const t_miembro *arr_mbr, const t_indice *idx_mbr) {
    t_acc_miembro acc[MAX_ALQUILERES];
    int           n = 0, i, j, encontrado;
    t_reg_indice  clave;
    int           pos;

    for (i = 0; i < cant_alq; i++) {
        if (arr_alq[i].alquileres_activos <= 0) continue;
        encontrado = 0;
        for (j = 0; j < n; j++) {
            if (acc[j].dni == arr_alq[i].dni) {
                acc[j].total += arr_alq[i].alquileres_activos;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            acc[n].dni   = arr_alq[i].dni;
            acc[n].total = arr_alq[i].alquileres_activos;
            n++;
        }
    }

    if (n == 0) { printf("No hay miembros con alquileres activos.\n"); return; }

    ordenamiento_generico(acc, (size_t)n, sizeof(t_acc_miembro), cmp_acc_miembro_desc);

    printf("\n%-12s %-30s %-12s\n", "DNI", "Miembro", "Sin devolver");
    printf("%-12s %-30s %-12s\n",
           "------------", "------------------------------", "------------");

    for (i = 0; i < n; i++) {
        const char *nombre = "(desconocido)";

        clave.dni = acc[i].dni;
        pos = indice_buscar(idx_mbr, &clave, idx_mbr->cantidad_elementos_actual,
                            sizeof(t_reg_indice), cmp_reg_indice);
        if (pos != NO_EXISTE)
            nombre = arr_mbr[((t_reg_indice *)idx_mbr->vindice)[pos].nro_reg].apellidos_nombres;

        printf("%-12ld %-30s %-12d\n", acc[i].dni, nombre, acc[i].total);
    }
}

void titulos_listar_mas_alquilados(const t_alquiler *arr_alq, int cant_alq,
                                   const t_titulo *arr_tit, const t_indice *idx_tit) {
    t_acc_titulo acc[MAX_ALQUILERES];
    int          n = 0, i, j, encontrado;
    t_reg_indice clave;
    int          pos;

    for (i = 0; i < cant_alq; i++) {
        if (arr_alq[i].total_alquileres <= 0) continue;
        encontrado = 0;
        for (j = 0; j < n; j++) {
            if (acc[j].id_pelicula == arr_alq[i].id_pelicula) {
                acc[j].total += arr_alq[i].total_alquileres;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            acc[n].id_pelicula = arr_alq[i].id_pelicula;
            acc[n].total       = arr_alq[i].total_alquileres;
            n++;
        }
    }

    if (n == 0) { printf("No hay alquileres registrados.\n"); return; }

    ordenamiento_generico(acc, (size_t)n, sizeof(t_acc_titulo), cmp_acc_titulo_desc);

    printf("\n%-6s %-30s %-12s\n", "ID", "Titulo", "Total alq.");
    printf("%-6s %-30s %-12s\n", "------", "------------------------------", "------------");

    for (i = 0; i < n; i++) {
        const char *titulo = "(desconocido)";

        clave.dni = (long)acc[i].id_pelicula;
        pos = indice_buscar(idx_tit, &clave, idx_tit->cantidad_elementos_actual,
                            sizeof(t_reg_indice), cmp_reg_indice);
        if (pos != NO_EXISTE)
            titulo = arr_tit[((t_reg_indice *)idx_tit->vindice)[pos].nro_reg].titulo;

        printf("%-6d %-30s %-12d\n", acc[i].id_pelicula, titulo, acc[i].total);
    }
}

int archivos_fechados_existen(t_fecha fp, const char *data_path) {
    char  path[256], fecha_str[12];
    FILE *f;

    fecha_a_string_archivo(fp, fecha_str);

    sprintf(path, "%s/miembros_%s.csv", data_path, fecha_str);
    f = fopen(path, "r");
    if (!f) return 0;
    fclose(f);

    sprintf(path, "%s/titulos_%s.csv", data_path, fecha_str);
    f = fopen(path, "r");
    if (!f) return 0;
    fclose(f);

    return 1;
}

void imprimir_menu(void) {
    printf("\n========================================\n");
    printf("     CINEFILIA - Menu de Operaciones\n");
    printf("========================================\n");
    printf("  a. Alta de miembro\n");
    printf("  b. Alta de titulo\n");
    printf("  c. Baja de miembro\n");
    printf("  d. Baja de titulo\n");
    printf("  e. Modificacion de miembro\n");
    printf("  f. Modificacion de titulo\n");
    printf("  g. Mostrar informacion de un miembro\n");
    printf("  h. Alquiler de un titulo\n");
    printf("  i. Listado de miembros ordenados por DNI\n");
    printf("  j. Listado miembros por Plan\n");
    printf("  k. Ver incidencias\n");
    printf("  m. Listado de morosos\n");
    printf("  n. Indice de alquileres\n");
    printf("  o. Listado de miembros menores activos\n");
    printf("  p. Listado de miembros por plan especifico\n");
    printf("  q. Listado de miembros proximos a la morosidad\n");
    printf("  r. Listado de miembros por sexo\n");
    printf("  s. Antiguedad de miembros (mas de N anios afiliados)\n");
    printf("  t. Peliculas sin stock\n");
    printf("  u. Peliculas por genero\n");
    printf("  v. Peliculas con stock bajo\n");
    printf("  w. Miembros con alquileres activos\n");
    printf("  x. Peliculas mas alquiladas\n");
    printf("  l. Salir\n");
    printf("========================================\n");
    printf("  Opcion: ");
}

void ver_incidencias(t_fecha fp, t_incidencias_miembros *inc_mbr,
                     t_incidencias_titulos *inc_tit) {
    char                   fecha_str[12], path[128];
    t_incidencias_miembros inc_m_tmp;
    t_incidencias_titulos  inc_t_tmp;
    int                    op;

    printf("\n--- Incidencias ---\n");
    printf("  1. Incidencias de miembros (sesion actual)\n");
    printf("  2. Incidencias de titulos  (sesion actual)\n");
    printf("  3. Cargar incidencias de miembros desde CSV\n");
    printf("  4. Cargar incidencias de titulos  desde CSV\n");
    printf("  Opcion: ");
    if (scanf("%d", &op) != 1) op = 0;
    getchar();

    fecha_a_string_archivo(fp, fecha_str);

    switch (op) {
    case 1:
        incidencias_miembros_imprimir(inc_mbr);
        break;
    case 2:
        incidencias_titulos_imprimir(inc_tit);
        break;
    case 3:
        printf("Archivo (Enter = incidencias_mbr_%s.csv): ", fecha_str);
        leer_linea_alq(path, sizeof(path));
        if (path[0] == '\0')
            sprintf(path, "incidencias_mbr_%s.csv", fecha_str);
        incidencias_miembros_init(&inc_m_tmp);
        if (incidencias_miembros_cargar_csv(&inc_m_tmp, path))
            incidencias_miembros_imprimir(&inc_m_tmp);
        else
            printf("No se pudo cargar '%s'.\n", path);
        break;
    case 4:
        printf("Archivo (Enter = incidencias_tit_%s.csv): ", fecha_str);
        leer_linea_alq(path, sizeof(path));
        if (path[0] == '\0')
            sprintf(path, "incidencias_tit_%s.csv", fecha_str);
        incidencias_titulos_init(&inc_t_tmp);
        if (incidencias_titulos_cargar_csv(&inc_t_tmp, path))
            incidencias_titulos_imprimir(&inc_t_tmp);
        else
            printf("No se pudo cargar '%s'.\n", path);
        break;
    default:
        printf("Opcion invalida.\n");
    }
}
