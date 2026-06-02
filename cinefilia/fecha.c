#include <stdio.h>
#include <string.h>
#include <time.h>
#include "fecha.h"

int es_bisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

int dias_en_mes(int mes, int anio) {
    static const int dias[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes < 1 || mes > 12) return 0;
    if (mes == 2 && es_bisiesto(anio)) return 29;
    return dias[mes];
}

int fecha_valida(t_fecha f) {
    if (f.anio < 1800 || f.anio > 9999) return 0;
    if (f.mes  < 1    || f.mes  > 12)   return 0;
    if (f.dia  < 1    || f.dia  > dias_en_mes(f.mes, f.anio)) return 0;
    return 1;
}

t_fecha fecha_actual(void) {
    time_t     t       = time(NULL);
    struct tm *tm_info = localtime(&t);
    t_fecha    f;
    f.dia  = tm_info->tm_mday;
    f.mes  = tm_info->tm_mon + 1;
    f.anio = tm_info->tm_year + 1900;
    return f;
}

int fecha_menor_igual(t_fecha f1, t_fecha f2) {
    if (f1.anio != f2.anio) return f1.anio < f2.anio;
    if (f1.mes  != f2.mes)  return f1.mes  < f2.mes;
    return f1.dia <= f2.dia;
}

int fecha_igual(t_fecha f1, t_fecha f2) {
    return (f1.dia == f2.dia && f1.mes == f2.mes && f1.anio == f2.anio);
}

int calcular_edad(t_fecha nacimiento, t_fecha proceso) {
    int edad = proceso.anio - nacimiento.anio;
    if (proceso.mes < nacimiento.mes ||
        (proceso.mes == nacimiento.mes && proceso.dia < nacimiento.dia)) {
        edad--;
    }
    return edad;
}

int fecha_desde_string(const char *str, t_fecha *f) {
    if (sscanf(str, "%d/%d/%d", &f->dia, &f->mes, &f->anio) != 3)
        return 0;
    return fecha_valida(*f);
}

void fecha_imprimir(t_fecha f) {
    printf("%02d/%02d/%04d", f.dia, f.mes, f.anio);
}

void fecha_a_string_archivo(t_fecha f, char *buf) {
    sprintf(buf, "%04d%02d%02d", f.anio, f.mes, f.dia);
}

t_fecha pedir_fecha_proceso(void) {
    char    buf[32];
    t_fecha fp;
    int     ok = 0;

    printf("Fecha de proceso (DD/MM/AAAA) o [Enter] para usar la fecha del sistema: ");
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin)) {
        int len = (int)strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';

        if (buf[0] == '\0') {
            fp = fecha_actual();
            ok = 1;
        } else {
            ok = fecha_desde_string(buf, &fp);
        }
    }

    if (!ok) {
        printf("Fecha invalida. Se usa la fecha del sistema.\n");
        fp = fecha_actual();
    }

    printf("Fecha de proceso: ");
    fecha_imprimir(fp);
    printf("\n");
    fflush(stdout);
    return fp;
}
