#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "titulo.h"

static void pedir_genero(char *genero) {
    int op;
    do {
        printf("  Genero:\n    1. Accion\n    2. Drama\n    3. Comedia\n    4. Terror\n  Opcion: ");
        if (scanf("%d", &op) != 1) op = 0;
        getchar();
    } while (op < 1 || op > 4);
    const char *generos[] = {"", "Accion", "Drama", "Comedia", "Terror"};
    strcpy(genero, generos[op]);
}

static void leer_linea(char *buf, int max) {
    if (fgets(buf, max, stdin)) {
        int len = (int)strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
    }
}

static int csv_split(char *linea, char **campos, int max) {
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

static void normalizar_genero(const char *src, char *dest) {
    int i;
    for (i = 0; src[i] && i < LEN_GENERO - 1; i++)
        dest[i] = (i == 0) ? (char)toupper((unsigned char)src[i])
                           : (char)tolower((unsigned char)src[i]);
    dest[i] = '\0';
}

int titulo_validar(const t_titulo *t) {
    if (!validar_generico(&t->id_pelicula, validar_id_pelicula)) return ERR_TIT_ID;
    if (!validar_generico(t->titulo,       validar_nombre))      return ERR_TIT_TITULO;
    if (!validar_generico(t->genero,       validar_genero))      return ERR_TIT_GENERO;
    if (!validar_generico(&t->stock,       validar_stock))       return ERR_TIT_STOCK;
    return -1;
}

int titulos_cargar_csv(const char *path, t_titulo *arr, int *cant,
                       t_indice *idx, t_incidencias_titulos *inc) {
    FILE        *f;
    char         linea[256];
    char        *campos[6];
    int          n, err;
    t_titulo     t;
    t_reg_indice ri;

    f = fopen(path, "r");
    if (!f) { 
        printf("Error: no se pudo abrir %s\n", path);
        fflush(stdout);
        return 0;
    }

    while (fgets(linea, sizeof(linea), f)) {
        if (linea[0] == '\n' || linea[0] == '#') continue;
        n = csv_split(linea, campos, 6);
        if (n < 4) continue;

        memset(&t, 0, sizeof(t));
        t.id_pelicula = atoi(campos[0]);
        normalizar_nombre(campos[1], t.titulo);
        normalizar_genero(campos[2], t.genero);
        t.stock  = atoi(campos[3]);
        t.estado = 'A';

        err = titulo_validar(&t);
        if (err != -1) {
            incidencias_titulos_agregar(inc, err, t.id_pelicula); continue;
        }

        ri.dni     = (long)t.id_pelicula;
        ri.nro_reg = (unsigned)*cant;
        if (indice_buscar(idx, &ri, idx->cantidad_elementos_actual,
                          sizeof(t_reg_indice), cmp_reg_indice) != NO_EXISTE) {
            incidencias_titulos_agregar(inc, ERR_TIT_ID, t.id_pelicula); continue;
        }

        if (*cant >= MAX_TITULOS) break;
        arr[*cant] = t;
        indice_insertar(idx, &ri, sizeof(t_reg_indice), cmp_reg_indice);
        (*cant)++;
    }

    fclose(f);
    return *cant;
}

int titulos_cargar_csv_fechado(const char *path, t_titulo *arr, int *cant,
                               t_indice *idx) {
    FILE        *f;
    char         linea[256];
    char        *campos[6];
    int          n;
    t_titulo     t;
    t_reg_indice ri;

    f = fopen(path, "r");
    if (!f) return 0;

    *cant = 0;
    while (fgets(linea, sizeof(linea), f) && *cant < MAX_TITULOS) {
        if (linea[0] == '\n' || linea[0] == '#') continue;
        n = csv_split(linea, campos, 6);
        if (n < 5) continue;

        memset(&t, 0, sizeof(t));
        t.id_pelicula = atoi(campos[0]);
        strncpy(t.titulo, campos[1], LEN_TITULO - 1);
        strncpy(t.genero, campos[2], LEN_GENERO - 1);
        t.stock  = atoi(campos[3]);
        t.estado = campos[4][0];

        arr[*cant] = t;
        ri.dni     = (long)t.id_pelicula;
        ri.nro_reg = (unsigned)*cant;
        if (t.estado == 'A')
            indice_insertar(idx, &ri, sizeof(t_reg_indice), cmp_reg_indice);
        (*cant)++;
    }

    fclose(f);
    return *cant;
}

int titulos_guardar_csv(const char *path, const t_titulo *arr, int cant) {
    FILE *f;
    int   i;

    f = fopen(path, "w");
    if (!f) return 0;

    for (i = 0; i < cant; i++)
        fprintf(f, "%d;%s;%s;%d;%c\n",
                arr[i].id_pelicula, arr[i].titulo,
                arr[i].genero, arr[i].stock, arr[i].estado);

    fclose(f);
    return 1;
}

int titulo_proximo_id(const t_titulo *arr, int cant) {
    int max_id = 0, i;
    for (i = 0; i < cant; i++)
        if (arr[i].id_pelicula > max_id) max_id = arr[i].id_pelicula;
    return max_id + 1;
}

/* ================================================================
   Alta interactiva
   ================================================================ */
void titulo_alta(t_titulo *arr, int *cant, t_indice *idx) {
    t_titulo     t;
    t_reg_indice ri;
    char         buf[128];
    int          err;

    memset(&t, 0, sizeof(t));
    t.id_pelicula = titulo_proximo_id(arr, *cant);
    printf("ID asignado automaticamente: %d\n", t.id_pelicula);

    printf("Titulo: ");
    leer_linea(buf, sizeof(buf));
    normalizar_nombre(buf, t.titulo);

    pedir_genero(t.genero);

    printf("Stock: ");
    leer_linea(buf, sizeof(buf));
    t.stock  = atoi(buf);
    t.estado = 'A';

    err = titulo_validar(&t);
    if (err != -1) {
        printf("Error de validacion (campo %d). Alta rechazada.\n", err); return;
    }
    if (*cant >= MAX_TITULOS) { printf("Capacidad maxima alcanzada.\n"); return; }

    arr[*cant] = t;
    ri.dni     = (long)t.id_pelicula;
    ri.nro_reg = (unsigned)*cant;
    indice_insertar(idx, &ri, sizeof(t_reg_indice), cmp_reg_indice);
    (*cant)++;
    printf("Titulo '%s' dado de alta (ID=%d).\n", t.titulo, t.id_pelicula);
}

/* ================================================================
   Baja logica
   ================================================================ */
void titulo_baja(t_titulo *arr, t_indice *idx) {
    int          id;
    char         buf[32];
    t_reg_indice ri;
    int          pos;

    printf("ID del titulo a dar de baja: ");
    leer_linea(buf, sizeof(buf));
    id = atoi(buf);

    ri.dni     = (long)id;
    ri.nro_reg = 0;
    pos = indice_buscar(idx, &ri, idx->cantidad_elementos_actual,
                        sizeof(t_reg_indice), cmp_reg_indice);
    if (pos == NO_EXISTE) { printf("ID %d no encontrado.\n", id); return; }

    ri = ((t_reg_indice *)idx->vindice)[pos];
    if (arr[ri.nro_reg].estado == 'B') { printf("El titulo ya esta dado de baja.\n"); return; }

    arr[ri.nro_reg].estado = 'B';
    indice_eliminar(idx, &ri, sizeof(t_reg_indice), cmp_reg_indice);
    printf("Titulo ID %d dado de baja.\n", id);
}

void titulo_modificar(t_titulo *arr, t_indice *idx) {
    int          id, op, err;
    char         buf[128];
    t_reg_indice ri;
    int          pos;
    t_titulo     tmp;

    printf("ID del titulo a modificar: ");
    leer_linea(buf, sizeof(buf));
    id = atoi(buf);

    ri.dni     = (long)id;
    ri.nro_reg = 0;
    pos = indice_buscar(idx, &ri, idx->cantidad_elementos_actual,
                        sizeof(t_reg_indice), cmp_reg_indice);
    if (pos == NO_EXISTE) { printf("ID %d no encontrado.\n", id); return; }

    ri  = ((t_reg_indice *)idx->vindice)[pos];
    tmp = arr[ri.nro_reg];

    printf("\n--- Datos actuales ---\n");
    titulo_mostrar(&tmp);

    printf("\nQue desea modificar?\n");
    printf("  1. Titulo\n  2. Genero\n  3. Stock\n  0. Cancelar\nOpcion: ");
    if (scanf("%d", &op) != 1) op = 0;
    getchar();

    switch (op) {
    case 1:
        printf("Nuevo titulo: "); leer_linea(buf, sizeof(buf));
        normalizar_nombre(buf, tmp.titulo); break;
    case 2: pedir_genero(tmp.genero); break;
    case 3:
        printf("Nuevo stock: "); leer_linea(buf, sizeof(buf));
        tmp.stock = atoi(buf); break;
    default: printf("Modificacion cancelada.\n"); return;
    }

    err = titulo_validar(&tmp);
    if (err != -1) {
        printf("Error de validacion. Modificacion descartada.\n"); return;
    }
    arr[ri.nro_reg] = tmp;
    printf("Titulo modificado exitosamente.\n");
}

void titulo_mostrar(const t_titulo *t) {
    printf("  ID     : %d\n",  t->id_pelicula);
    printf("  Titulo : %s\n",  t->titulo);
    printf("  Genero : %s\n",  t->genero);
    printf("  Stock  : %d\n",  t->stock);
    printf("  Estado : %c\n",  t->estado);
}

void titulo_mostrar_por_id(const t_titulo *arr, const t_indice *idx) {
    int          id;
    char         buf[32];
    t_reg_indice ri;
    int          pos;

    printf("ID del titulo a consultar: ");
    leer_linea(buf, sizeof(buf));
    id = atoi(buf);

    ri.dni     = (long)id;
    ri.nro_reg = 0;
    pos = indice_buscar(idx, &ri, idx->cantidad_elementos_actual,
                        sizeof(t_reg_indice), cmp_reg_indice);
    if (pos == NO_EXISTE) { printf("ID %d no encontrado.\n", id); return; }

    ri = ((t_reg_indice *)idx->vindice)[pos];
    printf("\n--- Informacion del Titulo ---\n");
    titulo_mostrar(&arr[ri.nro_reg]);
}
