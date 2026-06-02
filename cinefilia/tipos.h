#ifndef TIPOS_H_INCLUDED
#define TIPOS_H_INCLUDED

#define MAX_MIEMBROS      1000
#define MAX_TITULOS        500
#define MAX_ALQUILERES    5000

#define LEN_NOMBRE          61
#define LEN_CUIL            14   /* XX-XXXXXXXX-X\0 */
#define LEN_PLAN            11
#define LEN_GENERO          21
#define LEN_EMAIL           31
#define LEN_TITULO          61
#define LEN_CATEGORIA       10

typedef struct {
    int dia;
    int mes;
    int anio;
} t_fecha;

typedef struct {
    long    dni;
    char    cuil[LEN_CUIL];
    char    apellidos_nombres[LEN_NOMBRE];
    t_fecha fecha_nacimiento;
    char    sexo;                       /* 'F', 'M', 'O' */
    t_fecha fecha_afiliacion;
    char    categoria[LEN_CATEGORIA];   /* 'MENOR', 'ADULTO' */
    t_fecha fecha_ultima_cuota;
    char    estado;                     /* 'A', 'B' */
    char    plan[LEN_PLAN];             /* 'BASIC', 'PREMIUM', 'VIP', 'FAMILY' */
    char    email_tutor[LEN_EMAIL];
} t_miembro;

typedef struct {
    int  id_pelicula;
    char titulo[LEN_TITULO];
    char genero[LEN_GENERO];   /* 'Accion', 'Drama', 'Comedia', 'Terror' */
    int  stock;
    char estado;               /* 'A', 'B' */
} t_titulo;

typedef struct {
    long dni;
    int  id_pelicula;
    int  total_alquileres;
    int  alquileres_activos;
} t_alquiler;

#endif // TIPOS_H_INCLUDED
