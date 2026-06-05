#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "miembro.h"

static void pedir_plan(char *plan) {
    int op;
    do {
        printf("  Plan:\n    1. BASIC\n    2. PREMIUM\n    3. VIP\n    4. FAMILY\n  Opcion: ");
        if (scanf("%d", &op) != 1) op = 0;
        getchar();
    } while (op < 1 || op > 4);
    const char *planes[] = {"", "BASIC", "PREMIUM", "VIP", "FAMILY"};
    strcpy(plan, planes[op]);
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
    while (ptr >= campos[n - 1] && (*ptr == '\n' || *ptr == '\r')) *ptr-- = '\0';
    return n;
}

int miembro_validar(const t_miembro *m, t_fecha fp) {
    char nombre_norm[LEN_NOMBRE];

    if (!validar_generico(&m->dni,                validar_dni))    return ERR_MBR_DNI;
    normalizar_nombre(m->apellidos_nombres, nombre_norm);
    if (!validar_generico(nombre_norm,            validar_nombre)) return ERR_MBR_NOMBRE;
    if (!fecha_valida(m->fecha_nacimiento))                        return ERR_MBR_FECHA_NAC;
    if (calcular_edad(m->fecha_nacimiento, fp) < 10)               return ERR_MBR_FECHA_NAC;
    if (!validar_generico(&m->sexo,               validar_sexo))   return ERR_MBR_SEXO;
    if (!fecha_valida(m->fecha_afiliacion))                        return ERR_MBR_FECHA_AFIL;
    if (!fecha_menor_igual(m->fecha_nacimiento, m->fecha_afiliacion)) return ERR_MBR_FECHA_AFIL;
    if (!fecha_menor_igual(m->fecha_afiliacion, fp))               return ERR_MBR_FECHA_AFIL;
    if (!fecha_valida(m->fecha_ultima_cuota))                      return ERR_MBR_FECHA_CUOTA;
    if (!fecha_menor_igual(m->fecha_afiliacion, m->fecha_ultima_cuota)) return ERR_MBR_FECHA_CUOTA;
    if (!fecha_menor_igual(m->fecha_ultima_cuota, fp))             return ERR_MBR_FECHA_CUOTA;
    if (!validar_generico(m->plan,                validar_plan))   return ERR_MBR_PLAN;
    if (strcmp(m->categoria, "MENOR") == 0)
        if (!validar_email(m->email_tutor))                        return ERR_MBR_EMAIL;
    return -1;
}

static int miembro_validar_csv(const t_miembro *m, t_fecha fp) {
    int err = miembro_validar(m, fp);
    if (err == ERR_MBR_NOMBRE) return -1;
    return err;
}

int miembros_cargar_csv(const char *path, t_miembro *arr, int *cant,
                        t_indice *idx, t_incidencias_miembros *inc, t_fecha fp) {
    FILE        *f;
    char         linea[256];
    char        *campos[10];
    int          n, err, k;
    t_miembro    m;
    t_reg_indice ri;

    f = fopen(path, "r");
    if (!f) { 
        printf("Error: no se pudo abrir %s\n", path);
        fflush(stdout);
        return 0; 
    }

    while (fgets(linea, sizeof(linea), f)) {
        if (linea[0] == '\n' || linea[0] == '#') continue;
        n = csv_split(linea, campos, 10);
        if (n < 7) continue;

        memset(&m, 0, sizeof(m));
        m.dni = atol(campos[0]);

        char nombre_temp[LEN_NOMBRE];
        strncpy(nombre_temp, campos[1], LEN_NOMBRE - 1);
        nombre_temp[LEN_NOMBRE - 1] = '\0';
        normalizar_nombre(nombre_temp, m.apellidos_nombres);

        if (!fecha_desde_string(campos[2], &m.fecha_nacimiento)) {
            incidencias_miembros_agregar(inc, ERR_MBR_FECHA_NAC, m.dni); continue;
        }

        m.sexo = (char)toupper((unsigned char)campos[3][0]);

        if (!fecha_desde_string(campos[4], &m.fecha_afiliacion)) {
            incidencias_miembros_agregar(inc, ERR_MBR_FECHA_AFIL, m.dni); continue;
        }

        if (!fecha_desde_string(campos[5], &m.fecha_ultima_cuota)) {
            incidencias_miembros_agregar(inc, ERR_MBR_FECHA_CUOTA, m.dni); continue;
        }

        for (k = 0; campos[6][k] && k < (int)sizeof(m.plan) - 1; k++)
            m.plan[k] = (char)toupper((unsigned char)campos[6][k]);
        m.plan[k] = '\0';

        if (n >= 8 && campos[7][0] != '\0')
            strncpy(m.email_tutor, campos[7], LEN_EMAIL - 1);
        else
            m.email_tutor[0] = '\0';

        /* Categoria calculada a partir de la fecha de nacimiento */
        strcpy(m.categoria,
               (calcular_edad(m.fecha_nacimiento, fp) < 18) ? "MENOR" : "ADULTO");

        m.estado = 'A';
        calcular_cuil(m.dni, m.sexo, m.cuil);

        err = miembro_validar_csv(&m, fp);
        if (err != -1) {
            incidencias_miembros_agregar(inc, err, m.dni); continue;
        }

        ri.dni     = m.dni;
        ri.nro_reg = (unsigned)*cant;
        if (indice_buscar(idx, &ri, idx->cantidad_elementos_actual,
                          sizeof(t_reg_indice), cmp_reg_indice) != NO_EXISTE) {
            incidencias_miembros_agregar(inc, ERR_MBR_DNI, m.dni); continue;
        }

        if (*cant >= MAX_MIEMBROS) break;

        arr[*cant] = m;
        indice_insertar(idx, &ri, sizeof(t_reg_indice), cmp_reg_indice);
        (*cant)++;
    }

    fclose(f);
    return *cant;
}

int miembros_cargar_csv_fechado(const char *path, t_miembro *arr, int *cant,
                                t_indice *idx) {
    FILE        *f;
    char         linea[256];
    char        *campos[12];
    int          n;
    char         nombre_norm[LEN_NOMBRE];
    t_miembro    m;
    t_reg_indice ri;

    f = fopen(path, "r");
    if (!f) return 0;

    *cant = 0;
    while (fgets(linea, sizeof(linea), f) && *cant < MAX_MIEMBROS) {
        if (linea[0] == '\n' || linea[0] == '#') continue;
        n = csv_split(linea, campos, 12);
        if (n < 10) continue;

        memset(&m, 0, sizeof(m));
        m.dni = atol(campos[0]);
        strncpy(m.cuil,                campos[1], LEN_CUIL - 1);
        strncpy(m.apellidos_nombres,   campos[2], LEN_NOMBRE - 1);
        m.apellidos_nombres[LEN_NOMBRE - 1] = '\0';
        char nombre_norm[LEN_NOMBRE];
        normalizar_nombre(m.apellidos_nombres, nombre_norm);
        strncpy(m.apellidos_nombres, nombre_norm, LEN_NOMBRE - 1);
        m.apellidos_nombres[LEN_NOMBRE - 1] = '\0';
        fecha_desde_string(campos[3], &m.fecha_nacimiento);
        m.sexo = campos[4][0];
        fecha_desde_string(campos[5], &m.fecha_afiliacion);
        strncpy(m.categoria,           campos[6], LEN_CATEGORIA - 1);
        fecha_desde_string(campos[7], &m.fecha_ultima_cuota);
        m.estado = campos[8][0];
        strncpy(m.plan,                campos[9], LEN_PLAN - 1);
        if (n >= 11) strncpy(m.email_tutor, campos[10], LEN_EMAIL - 1);

        arr[*cant] = m;
        ri.dni     = m.dni;
        ri.nro_reg = (unsigned)*cant;
        if (m.estado == 'A')
            indice_insertar(idx, &ri, sizeof(t_reg_indice), cmp_reg_indice);
        (*cant)++;
    }

    fclose(f);
    return *cant;
}

int miembros_guardar_csv(const char *path, const t_miembro *arr, int cant) {
    FILE *f;
    int   i;

    f = fopen(path, "w");
    if (!f) return 0;

    for (i = 0; i < cant; i++) {
        fprintf(f, "%ld;%s;%s;%02d/%02d/%04d;%c;%02d/%02d/%04d;%s;%02d/%02d/%04d;%c;%s;%s\n",
                arr[i].dni,
                arr[i].cuil,
                arr[i].apellidos_nombres,
                arr[i].fecha_nacimiento.dia, arr[i].fecha_nacimiento.mes,  arr[i].fecha_nacimiento.anio,
                arr[i].sexo,
                arr[i].fecha_afiliacion.dia, arr[i].fecha_afiliacion.mes,  arr[i].fecha_afiliacion.anio,
                arr[i].categoria,
                arr[i].fecha_ultima_cuota.dia, arr[i].fecha_ultima_cuota.mes, arr[i].fecha_ultima_cuota.anio,
                arr[i].estado,
                arr[i].plan,
                arr[i].email_tutor);
    }

    fclose(f);
    return 1;
}

void miembro_alta(t_miembro *arr, int *cant, t_indice *idx, t_fecha fp) {
    t_miembro    m;
    t_reg_indice ri;
    char         buf[128];
    int          err, edad;

    memset(&m, 0, sizeof(m));

    printf("DNI: ");
    leer_linea(buf, sizeof(buf));
    m.dni = atol(buf);

    ri.dni     = m.dni;
    ri.nro_reg = 0;
    if (indice_buscar(idx, &ri, idx->cantidad_elementos_actual,
                      sizeof(t_reg_indice), cmp_reg_indice) != NO_EXISTE) {
        printf("Error: el DNI %ld ya existe.\n", m.dni); return;
    }
    if (!validar_generico(&m.dni, validar_dni)) {
        printf("Error: DNI invalido (debe estar entre 1000000 y 100000000).\n"); return;
    }

    printf("Apellidos y Nombres: ");
    leer_linea(buf, sizeof(buf));
    normalizar_nombre(buf, m.apellidos_nombres);

    printf("Fecha de Nacimiento (DD/MM/AAAA): ");
    leer_linea(buf, sizeof(buf));
    if (!fecha_desde_string(buf, &m.fecha_nacimiento)) {
        printf("Error: fecha invalida.\n"); return;
    }

    edad = calcular_edad(m.fecha_nacimiento, fp);
    if (edad < 10) { printf("Error: debe tener al menos 10 anios.\n"); return; }
    strcpy(m.categoria, (edad < 18) ? "MENOR" : "ADULTO");
    printf("Categoria asignada: %s\n", m.categoria);

    printf("Sexo (F/M/O): ");
    leer_linea(buf, sizeof(buf));
    m.sexo = (char)toupper((unsigned char)buf[0]);

    printf("Fecha de Afiliacion (DD/MM/AAAA): ");
    leer_linea(buf, sizeof(buf));
    if (!fecha_desde_string(buf, &m.fecha_afiliacion)) {
        printf("Error: fecha invalida.\n"); return;
    }

    printf("Fecha Ultima Cuota (DD/MM/AAAA): ");
    leer_linea(buf, sizeof(buf));
    if (!fecha_desde_string(buf, &m.fecha_ultima_cuota)) {
        printf("Error: fecha invalida.\n"); return;
    }

    pedir_plan(m.plan);

    if (strcmp(m.categoria, "MENOR") == 0) {
        printf("Email del Tutor: ");
        leer_linea(m.email_tutor, LEN_EMAIL);
    }

    m.estado = 'A';
    calcular_cuil(m.dni, m.sexo, m.cuil);

    err = miembro_validar(&m, fp);
    if (err != -1) {
        printf("Error de validacion (campo %d). Alta rechazada.\n", err); return;
    }

    if (*cant >= MAX_MIEMBROS) { printf("Capacidad maxima alcanzada.\n"); return; }

    arr[*cant] = m;
    ri.nro_reg = (unsigned)*cant;
    indice_insertar(idx, &ri, sizeof(t_reg_indice), cmp_reg_indice);
    (*cant)++;
    printf("Miembro dado de alta. CUIL: %s\n", m.cuil);
}

void miembro_baja(t_miembro *arr, t_indice *idx, t_fecha fp) {
    long         dni;
    char         buf[32];
    t_reg_indice ri;
    int          pos;

    (void)fp;
    printf("DNI del miembro a dar de baja: ");
    leer_linea(buf, sizeof(buf));
    dni = atol(buf);

    ri.dni     = dni;
    ri.nro_reg = 0;
    pos = indice_buscar(idx, &ri, idx->cantidad_elementos_actual,
                        sizeof(t_reg_indice), cmp_reg_indice);
    if (pos == NO_EXISTE) { printf("DNI %ld no encontrado.\n", dni); return; }

    ri = ((t_reg_indice *)idx->vindice)[pos];
    if (arr[ri.nro_reg].estado == 'B') { printf("El miembro ya esta dado de baja.\n"); return; }

    arr[ri.nro_reg].estado = 'B';
    indice_eliminar(idx, &ri, sizeof(t_reg_indice), cmp_reg_indice);
    printf("Miembro %ld dado de baja.\n", dni);
}

void miembro_modificar(t_miembro *arr, t_indice *idx, t_fecha fp) {
    long         dni;
    char         buf[128];
    t_reg_indice ri;
    int          pos, op, err;
    t_miembro    tmp;

    printf("DNI del miembro a modificar: ");
    leer_linea(buf, sizeof(buf));
    dni = atol(buf);

    ri.dni     = dni;
    ri.nro_reg = 0;
    pos = indice_buscar(idx, &ri, idx->cantidad_elementos_actual,
                        sizeof(t_reg_indice), cmp_reg_indice);
    if (pos == NO_EXISTE) { printf("DNI %ld no encontrado.\n", dni); return; }

    ri  = ((t_reg_indice *)idx->vindice)[pos];
    tmp = arr[ri.nro_reg];

    printf("\n--- Datos actuales ---\n");
    miembro_mostrar(&tmp);

    printf("\nQue desea modificar?\n");
    printf("  1. Apellidos y Nombres\n  2. Fecha Nacimiento\n  3. Sexo\n");
    printf("  4. Fecha Afiliacion\n  5. Fecha Ultima Cuota\n  6. Plan\n");
    printf("  7. Email Tutor\n  0. Cancelar\nOpcion: ");
    if (scanf("%d", &op) != 1) op = 0;
    getchar();

    switch (op) {
    case 1:
        printf("Nuevo nombre: "); leer_linea(buf, sizeof(buf));
        normalizar_nombre(buf, tmp.apellidos_nombres); break;
    case 2:
        printf("Nueva fecha nacimiento (DD/MM/AAAA): "); leer_linea(buf, sizeof(buf));
        if (!fecha_desde_string(buf, &tmp.fecha_nacimiento)) {
            printf("Fecha invalida.\n"); return;
        }
        strcpy(tmp.categoria,
               (calcular_edad(tmp.fecha_nacimiento, fp) < 18) ? "MENOR" : "ADULTO");
        break;
    case 3:
        printf("Nuevo sexo (F/M/O): "); leer_linea(buf, sizeof(buf));
        tmp.sexo = (char)toupper((unsigned char)buf[0]);
        calcular_cuil(tmp.dni, tmp.sexo, tmp.cuil); break;
    case 4:
        printf("Nueva fecha afiliacion (DD/MM/AAAA): "); leer_linea(buf, sizeof(buf));
        if (!fecha_desde_string(buf, &tmp.fecha_afiliacion)) {
            printf("Fecha invalida.\n"); return;
        }
        break;
    case 5:
        printf("Nueva fecha ultima cuota (DD/MM/AAAA): "); leer_linea(buf, sizeof(buf));
        if (!fecha_desde_string(buf, &tmp.fecha_ultima_cuota)) {
            printf("Fecha invalida.\n"); return;
        }
        break;
    case 6: pedir_plan(tmp.plan); break;
    case 7:
        printf("Nuevo email tutor: "); leer_linea(tmp.email_tutor, LEN_EMAIL); break;
    default: printf("Modificacion cancelada.\n"); return;
    }

    err = miembro_validar(&tmp, fp);
    if (err != -1) {
        printf("Error de validacion (campo %d). Modificacion descartada.\n", err); return;
    }
    arr[ri.nro_reg] = tmp;
    printf("Miembro modificado exitosamente.\n");
}

void miembro_mostrar(const t_miembro *m) {
    printf("  DNI        : %ld\n",   m->dni);
    printf("  CUIL       : %s\n",    m->cuil);
    printf("  Nombre     : %s\n",    m->apellidos_nombres);
    printf("  Nacimiento : "); fecha_imprimir(m->fecha_nacimiento); printf("\n");
    printf("  Sexo       : %c\n",    m->sexo);
    printf("  Afiliacion : "); fecha_imprimir(m->fecha_afiliacion); printf("\n");
    printf("  Categoria  : %s\n",    m->categoria);
    printf("  Ult. Cuota : "); fecha_imprimir(m->fecha_ultima_cuota); printf("\n");
    printf("  Estado     : %c\n",    m->estado);
    printf("  Plan       : %s\n",    m->plan);
    if (m->email_tutor[0])
        printf("  Email Tutor: %s\n", m->email_tutor);
}

void miembro_mostrar_por_dni(const t_miembro *arr, const t_indice *idx) {
    long         dni;
    char         buf[32];
    t_reg_indice ri;
    int          pos;

    printf("DNI a consultar: ");
    leer_linea(buf, sizeof(buf));
    dni = atol(buf);

    ri.dni     = dni;
    ri.nro_reg = 0;
    pos = indice_buscar(idx, &ri, idx->cantidad_elementos_actual,
                        sizeof(t_reg_indice), cmp_reg_indice);
    if (pos == NO_EXISTE) { printf("Miembro %ld no encontrado.\n", dni); return; }

    ri = ((t_reg_indice *)idx->vindice)[pos];
    printf("\n--- Informacion del Miembro ---\n");
    miembro_mostrar(&arr[ri.nro_reg]);
}

void miembros_listar_por_dni(const t_miembro *arr, const t_indice *idx) {
    unsigned      i;
    t_reg_indice *ri;

    if (indice_vacio(idx) == OK) { printf("No hay miembros activos.\n"); return; }

    printf("\n%-12s %-30s %-10s %-6s\n", "DNI", "Apellidos y Nombres", "Plan", "Estado");
    printf("%-12s %-30s %-10s %-6s\n",
           "------------", "------------------------------", "----------", "------");

    for (i = 0; i < idx->cantidad_elementos_actual; i++) {
        ri = (t_reg_indice *)idx->vindice + i;
        printf("%-12ld %-30s %-10s %-6c\n",
               arr[ri->nro_reg].dni, arr[ri->nro_reg].apellidos_nombres,
               arr[ri->nro_reg].plan, arr[ri->nro_reg].estado);
    }
}

static int cmp_por_nombre(const void *a, const void *b) {
    return strcmp(((const t_miembro *)a)->apellidos_nombres,
                  ((const t_miembro *)b)->apellidos_nombres);
}

void miembros_listar_por_plan(const t_miembro *arr, int cant) {
    t_miembro copia[MAX_MIEMBROS];
    int       n = 0, i;
    char      dni_str[13];

    for (i = 0; i < cant; i++)
        if (arr[i].estado == 'A') copia[n++] = arr[i];
    if (n == 0) { printf("No hay miembros activos.\n"); return; }

    qsort(copia, (size_t)n, sizeof(t_miembro), cmp_por_nombre);

    printf("\n%-30s %-12s %-12s %-12s %-12s\n",
           "Plan / Indice", "(BASIC)", "(PREMIUM)", "(VIP)", "(FAMILY)");
    printf("%-30s %-12s %-12s %-12s %-12s\n",
           "------------------------------",
           "------------", "------------", "------------", "------------");

    for (i = 0; i < n; i++) {
        char col_b[13]="0", col_p[13]="0", col_v[13]="0", col_f[13]="0";
        sprintf(dni_str, "%ld", copia[i].dni);
        if (strcmp(copia[i].plan, "BASIC")   == 0) strcpy(col_b, dni_str);
        if (strcmp(copia[i].plan, "PREMIUM") == 0) strcpy(col_p, dni_str);
        if (strcmp(copia[i].plan, "VIP")     == 0) strcpy(col_v, dni_str);
        if (strcmp(copia[i].plan, "FAMILY")  == 0) strcpy(col_f, dni_str);
        printf("%-30s %-12s %-12s %-12s %-12s\n",
               copia[i].apellidos_nombres, col_b, col_p, col_v, col_f);
    }
}
