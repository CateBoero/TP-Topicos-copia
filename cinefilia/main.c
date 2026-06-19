/* ============================================================
   main.c - Sistema Cinefilia
   UNLAM - Topicos de Programacion - 1C 2026
   Alumnos:
   Caterina Milagros Boero
   Nahuel Agustin Rigoli
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"
#include "fecha.h"
#include "indice.h"
#include "funciones.h"
#include "miembro.h"
#include "titulo.h"


t_miembro  arr_miembros[MAX_MIEMBROS];
int        cant_miembros   = 0;

t_titulo   arr_titulos[MAX_TITULOS];
int        cant_titulos    = 0;

t_alquiler arr_alquileres[MAX_ALQUILERES];
int        cant_alquileres = 0;

t_indice   indice_miembros;
t_indice   indice_titulos;

t_incidencias_miembros inc_miembros;
t_incidencias_titulos  inc_titulos;

t_fecha fecha_proceso;


int main(void) {
    char opcion[4];
    char fecha_str[12], path[128], data_path[256];
    int  path_len;

    printf("=====================================================\n");
    printf("   CINEFILIA - Sistema de Gestion del Videoclub     \n");
    printf("   UNLAM - Topicos de Programacion - 1C 2026        \n");
    printf("=====================================================\n\n");

    /* 0. Ruta de archivos de datos */
    printf("Ruta de archivos CSV (Enter = Docs/): ");
    fflush(stdout);
    if (fgets(data_path, sizeof(data_path), stdin)) {
        int len = (int)strlen(data_path);
        if (len > 0 && data_path[len - 1] == '\n') data_path[len - 1] = '\0';
    }

    if (data_path[0] == '\0') {
        strcpy(data_path, "Docs");
    }

    path_len = (int)strlen(data_path);
    if (path_len > 0 && data_path[path_len - 1] == '/') {
        data_path[path_len - 1] = '\0';
    }

    printf("Usando ruta: %s\n\n", data_path);
    fflush(stdout);

    /* 1. Fecha de proceso */
    fecha_proceso = pedir_fecha_proceso();
    fflush(stdout);
    fecha_a_string_archivo(fecha_proceso, fecha_str);

    /* 2. Crear indices */
    printf("Inicializando indices...\n");
    fflush(stdout);
    indice_crear(&indice_miembros, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));
    indice_crear(&indice_titulos,  CANTIDAD_ELEMENTOS, sizeof(t_reg_indice));

    /* 3. Inicializar incidencias */
    printf("Inicializando incidencias...\n");
    fflush(stdout);
    incidencias_miembros_init(&inc_miembros);
    incidencias_titulos_init(&inc_titulos);

    /* 4. Cargar datos: CSV fechados si existen, si no CSV originales */
    printf("\nBuscando archivos fechados...\n");
    fflush(stdout);
    if (archivos_fechados_existen(fecha_proceso, data_path)) {
        printf("\nArchivos de sesion encontrados. Cargando...\n");
        fflush(stdout);

        sprintf(path, "%s/miembros_%s.csv", data_path, fecha_str);
        miembros_cargar_csv_fechado(path, arr_miembros, &cant_miembros, &indice_miembros);

        sprintf(path, "%s/titulos_%s.csv", data_path, fecha_str);
        titulos_cargar_csv_fechado(path, arr_titulos, &cant_titulos, &indice_titulos);

        sprintf(path, "%s/alquileres_%s.csv", data_path, fecha_str);
        alquileres_cargar_csv(path, arr_alquileres, &cant_alquileres);

        sprintf(path, "%s/incidencias_mbr_%s.csv", data_path, fecha_str);
        incidencias_miembros_cargar_csv(&inc_miembros, path);

        sprintf(path, "%s/incidencias_tit_%s.csv", data_path, fecha_str);
        incidencias_titulos_cargar_csv(&inc_titulos, path);

        printf("  Miembros   : %d\n", cant_miembros);
        printf("  Titulos    : %d\n", cant_titulos);
        printf("  Alquileres : %d\n", cant_alquileres);
    } else {
        printf("\nCargando desde archivos CSV originales...\n");
        fflush(stdout);
        sprintf(path, "%s/miembros.csv", data_path);
        miembros_cargar_csv(path, arr_miembros, &cant_miembros,
                            &indice_miembros, &inc_miembros, fecha_proceso);
        sprintf(path, "%s/titulos.csv", data_path);
        titulos_cargar_csv(path, arr_titulos, &cant_titulos,
                           &indice_titulos, &inc_titulos);
        printf("  Miembros cargados : %d\n", cant_miembros);
        printf("  Titulos cargados  : %d\n", cant_titulos);
        fflush(stdout);
    }

    /* 5. Mostrar incidencias de la carga */
    //incidencias_miembros_imprimir(&inc_miembros);
    //incidencias_titulos_imprimir(&inc_titulos);

    /* 6. Menu de operaciones */
    do {
        imprimir_menu();

        if (fgets(opcion, sizeof(opcion), stdin)) {
            int len = (int)strlen(opcion);
            if (len > 0 && opcion[len - 1] == '\n') opcion[len - 1] = '\0';
        }

        switch (opcion[0]) {
        case 'a': case 'A':
            printf("\n--- Alta de Miembro ---\n");
            miembro_alta(arr_miembros, &cant_miembros, &indice_miembros, fecha_proceso);
            break;
        case 'b': case 'B':
            printf("\n--- Alta de Titulo ---\n");
            titulo_alta(arr_titulos, &cant_titulos, &indice_titulos);
            break;
        case 'c': case 'C':
            printf("\n--- Baja de Miembro ---\n");
            miembro_baja(arr_miembros, &indice_miembros, fecha_proceso);
            break;
        case 'd': case 'D':
            printf("\n--- Baja de Titulo ---\n");
            titulo_baja(arr_titulos, &indice_titulos);
            break;
        case 'e': case 'E':
            printf("\n--- Modificacion de Miembro ---\n");
            miembro_modificar(arr_miembros, &indice_miembros, fecha_proceso);
            break;
        case 'f': case 'F':
            printf("\n--- Modificacion de Titulo ---\n");
            titulo_modificar(arr_titulos, &indice_titulos);
            break;
        case 'g': case 'G':
            printf("\n--- Informacion de Miembro ---\n");
            miembro_mostrar_por_dni(arr_miembros, &indice_miembros);
            break;
        case 'h': case 'H':
            printf("\n--- Alquiler / Devolucion ---\n");
            printf("  1. Alquilar\n  2. Devolver\n  Opcion: ");
            {
                char sub[4];
                if (fgets(sub, sizeof(sub), stdin)) {
                    int len = (int)strlen(sub);
                    if (len > 0 && sub[len - 1] == '\n') sub[len - 1] = '\0';
                }
                if (sub[0] == '1')
                    alquiler_registrar(arr_alquileres, &cant_alquileres,
                                       arr_miembros, &indice_miembros,
                                       arr_titulos,  &indice_titulos);
                else if (sub[0] == '2')
                    alquiler_devolver(arr_alquileres, cant_alquileres,
                                      arr_titulos, &indice_titulos);
            }
            break;
        case 'i': case 'I':
            printf("\n--- Listado por DNI ---\n");
            miembros_listar_por_dni(arr_miembros, &indice_miembros);
            break;
        case 'j': case 'J':
            printf("\n--- Listado por Plan ---\n");
            miembros_listar_por_plan(arr_miembros, cant_miembros);
            break;
        case 'k': case 'K':
            ver_incidencias(fecha_proceso, &inc_miembros, &inc_titulos);
            break;
        case 'm': case 'M':
            printf("\n--- Listado de Morosos ---\n");
            miembros_listar_morosos(arr_miembros, cant_miembros, fecha_proceso);
            break;
        case 'n': case 'N':
            printf("\n--- Indice de Alquileres ---\n");
            alquileres_listar_indice(arr_alquileres, cant_alquileres,
                                     arr_miembros, &indice_miembros,
                                     arr_titulos,  &indice_titulos);
            break;
        case 'o': case 'O':
            printf("\n--- Miembros Menores Activos ---\n");
            miembros_listar_menores(arr_miembros, cant_miembros);
            break;
        case 'p': case 'P':
            printf("\n--- Miembros por Plan Especifico ---\n");
            {
                char plan_buf[32];
                printf("Plan (BASIC/PREMIUM/VIP/FAMILY): ");
                if (fgets(plan_buf, sizeof(plan_buf), stdin)) {
                    int len = (int)strlen(plan_buf);
                    if (len > 0 && plan_buf[len - 1] == '\n') plan_buf[len - 1] = '\0';
                }
                miembros_listar_por_plan_especifico(arr_miembros, cant_miembros, plan_buf);
            }
            break;
        case 'q': case 'Q':
            printf("\n--- Miembros Proximos a la Morosidad ---\n");
            miembros_listar_proximos_morosidad(arr_miembros, cant_miembros, fecha_proceso);
            break;
        case 'r': case 'R':
            printf("\n--- Miembros por Sexo ---\n");
            {
                char sexo_buf[8];
                printf("Sexo (F/M/O): ");
                if (fgets(sexo_buf, sizeof(sexo_buf), stdin)) {
                    int len = (int)strlen(sexo_buf);
                    if (len > 0 && sexo_buf[len - 1] == '\n') sexo_buf[len - 1] = '\0';
                }
                miembros_listar_por_sexo(arr_miembros, cant_miembros, sexo_buf[0]);
            }
            break;
        case 's': case 'S':
            printf("\n--- Antiguedad de Miembros ---\n");
            {
                char buf_anios[16];
                int  anios;
                printf("Mas de cuantos anios de afiliacion: ");
                if (fgets(buf_anios, sizeof(buf_anios), stdin)) {
                    int len = (int)strlen(buf_anios);
                    if (len > 0 && buf_anios[len - 1] == '\n') buf_anios[len - 1] = '\0';
                }
                anios = atoi(buf_anios);
                miembros_listar_antiguedad(arr_miembros, cant_miembros, anios, fecha_proceso);
            }
            break;
        case 't': case 'T':
            printf("\n--- Peliculas sin Stock ---\n");
            titulos_listar_sin_stock(arr_titulos, cant_titulos);
            break;
        case 'u': case 'U':
            printf("\n--- Peliculas por Genero ---\n");
            {
                char genero_buf[32];
                printf("Genero (Accion/Drama/Comedia/Terror): ");
                if (fgets(genero_buf, sizeof(genero_buf), stdin)) {
                    int len = (int)strlen(genero_buf);
                    if (len > 0 && genero_buf[len - 1] == '\n') genero_buf[len - 1] = '\0';
                }
                titulos_listar_por_genero(arr_titulos, cant_titulos, genero_buf);
            }
            break;
        case 'v': case 'V':
            printf("\n--- Peliculas con Stock Bajo ---\n");
            {
                char buf_stock[16];
                int  n_stock;
                printf("Stock maximo a considerar: ");
                if (fgets(buf_stock, sizeof(buf_stock), stdin)) {
                    int len = (int)strlen(buf_stock);
                    if (len > 0 && buf_stock[len - 1] == '\n') buf_stock[len - 1] = '\0';
                }
                n_stock = atoi(buf_stock);
                titulos_listar_stock_bajo(arr_titulos, cant_titulos, n_stock);
            }
            break;
        case 'w': case 'W':
            printf("\n--- Miembros con Alquileres Activos ---\n");
            miembros_listar_con_alquileres_activos(arr_alquileres, cant_alquileres,
                                                    arr_miembros, &indice_miembros);
            break;
        case 'x': case 'X':
            printf("\n--- Peliculas mas Alquiladas ---\n");
            titulos_listar_mas_alquilados(arr_alquileres, cant_alquileres,
                                          arr_titulos, &indice_titulos);
            break;
        case 'y': case 'Y':
            menu_extra_defensa(arr_miembros, cant_miembros, &indice_miembros,
                               arr_titulos, cant_titulos,
                               arr_alquileres, cant_alquileres, fecha_proceso);
            break;
        case 'l': case 'L':
            printf("\nSaliendo...\n");
            break;
        default:
            printf("Opcion invalida.\n");
            break;
        }

    } while (opcion[0] != 'l' && opcion[0] != 'L');

    /* 7. Guardar en CSV (ordenar incidencias, luego escribir) */
    printf("\nGuardando datos en disco...\n");
    incidencias_ordenar_miembros(&inc_miembros);
    incidencias_ordenar_titulos(&inc_titulos);
    sprintf(path, "%s/miembros_%s.csv",         data_path, fecha_str);
    miembros_guardar_csv(path, arr_miembros, cant_miembros);
    sprintf(path, "%s/titulos_%s.csv",          data_path, fecha_str);
    titulos_guardar_csv(path, arr_titulos, cant_titulos);
    sprintf(path, "%s/alquileres_%s.csv",       data_path, fecha_str);
    alquileres_guardar_csv(path, arr_alquileres, cant_alquileres);
    sprintf(path, "%s/incidencias_mbr_%s.csv",  data_path, fecha_str);
    incidencias_miembros_guardar_csv(&inc_miembros, path);
    sprintf(path, "%s/incidencias_tit_%s.csv",  data_path, fecha_str);
    incidencias_titulos_guardar_csv(&inc_titulos, path);
    printf("Archivos generados:\n");
    printf("  miembros_%s.csv\n",         fecha_str);
    printf("  titulos_%s.csv\n",          fecha_str);
    printf("  alquileres_%s.csv\n",       fecha_str);
    printf("  incidencias_mbr_%s.csv\n",  fecha_str);
    printf("  incidencias_tit_%s.csv\n",  fecha_str);

    /* 8. Liberar indices */
    indice_vaciar(&indice_miembros);
    indice_vaciar(&indice_titulos);

    printf("\nHasta luego.\n");
    return EXIT_SUCCESS;
}
