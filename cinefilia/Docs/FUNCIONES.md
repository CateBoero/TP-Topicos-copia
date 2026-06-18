# Cinefilia — Guia de funciones del TP

Este documento explica que hace cada funcion del proyecto, donde esta
definida y como se usa, agrupadas por archivo. Sirve como referencia
para repasar antes de la defensa.

> Convencion general del proyecto: los listados activos se calculan
> sobre `estado == 'A'`; las bajas son logicas (`estado = 'B'`), nunca
> se borra un registro del arreglo. Todas las funciones de listado que
> ordenan, copian primero los registros a un arreglo temporal y ordenan
> la copia (nunca reordenan el arreglo original ni el indice).

---

## 1. `tipos.h` — Estructuras y constantes

No tiene funciones, pero define los tipos que usa todo el programa:

- `t_fecha { dia, mes, anio }`
- `t_miembro { dni, cuil, apellidos_nombres, fecha_nacimiento, sexo, fecha_afiliacion, categoria, fecha_ultima_cuota, estado, plan, email_tutor }`
- `t_titulo { id_pelicula, titulo, genero, stock, estado }`
- `t_alquiler { dni, id_pelicula, total_alquileres, alquileres_activos }`
- Constantes de tamano de arreglos (`MAX_MIEMBROS`, `MAX_TITULOS`, `MAX_ALQUILERES`) y de strings (`LEN_*`).

---

## 2. `fecha.c` / `fecha.h` — Manejo de fechas

| Funcion | Que hace | Como se usa |
|---|---|---|
| `es_bisiesto(anio)` | Indica si un anio es bisiesto. | Uso interno de `dias_en_mes`. |
| `dias_en_mes(mes, anio)` | Dias que tiene un mes dado (considera bisiestos). | Uso interno de `fecha_valida`. |
| `fecha_valida(f)` | Verifica que dia/mes/anio sean coherentes (1800-9999, mes 1-12, dia segun el mes). | Se llama antes de aceptar cualquier fecha ingresada por teclado o leida de CSV. |
| `fecha_actual()` | Devuelve la fecha del sistema (`time()`/`localtime()`). | Se usa si el usuario no ingresa fecha de proceso. |
| `fecha_menor_igual(f1, f2)` | `1` si `f1 <= f2`. | Validaciones cruzadas (ej: afiliacion <= fecha de proceso). |
| `fecha_igual(f1, f2)` | `1` si son la misma fecha. | Usada en comparadores de orden (`cmp_por_fecha_*`). |
| `calcular_edad(nacimiento, proceso)` | Anios completos entre dos fechas (resta anios y corrige si todavia no paso el mes/dia). | Calcula edad de un socio, **y tambien se reutiliza para calcular antiguedad de afiliacion** (`miembros_listar_antiguedad`), pasandole `fecha_afiliacion` en lugar de `fecha_nacimiento`. |
| `fecha_diferencia_dias(f1, f2)` | Cantidad de dias entre `f1` y `f2` (`f2 - f1`), via numero de dia juliano. | Base de "Listado de morosos" y "Proximos a la morosidad". |
| `fecha_desde_string(str, *f)` | Parsea `"DD/MM/AAAA"` a `t_fecha`; devuelve si es valida. | Lectura de CSV y de inputs por teclado. |
| `fecha_imprimir(f)` | Imprime `DD/MM/AAAA`. | Usada en todos los listados que muestran una fecha. |
| `fecha_a_string_archivo(f, buf)` | Formatea como `AAAAMMDD` (para nombres de archivo). | Genera `miembros_AAAAMMDD.csv`, etc. |
| `pedir_fecha_proceso()` | Pide la fecha de proceso por teclado; si el usuario presiona Enter usa la fecha del sistema. | Se llama una sola vez al iniciar `main()`. |

---

## 3. `indice.c` / `indice.h` — TDA Indice (catedra)

Estructura `t_reg_indice { nro_reg, dni }`: registro liviano que vincula
una **clave** (`dni`) con la **posicion** (`nro_reg`) dentro del arreglo
real (`arr_miembros`, `arr_titulos`, etc.). El campo `dni` se reutiliza
como clave generica: para miembros es el DNI, para titulos es el
`id_pelicula`.

`t_indice { vindice, cantidad_elementos_actual, cantidad_elementos_maxima }`:
arreglo dinamico de `t_reg_indice`, siempre mantenido **ordenado** por
la clave.

| Funcion | Que hace | Como se usa |
|---|---|---|
| `indice_crear(idx, nmemb, tamanyo)` | Reserva memoria para `nmemb` elementos. | Se llama una vez en `main()` para `indice_miembros` e `indice_titulos`. |
| `indice_redimensionar(idx, nmemb, tamanyo)` | Hace `realloc` a `nmemb` elementos. | Uso interno de `indice_insertar` cuando el indice esta lleno (crece x1.3). |
| `indice_insertar(idx, registro, tamanyo, cmp)` | Inserta manteniendo el orden (busqueda lineal de posicion + desplazamiento). | Cada alta de miembro/titulo, y construccion de indices transitorios (ej: indice de alquileres). |
| `indice_eliminar(idx, registro, tamanyo, cmp)` | Busca y quita un registro, desplazando a la izquierda. | Bajas logicas de miembro/titulo (se sigue viendo en el arreglo real con `estado='B'`, pero sale del indice). |
| `indice_buscar(idx, registro, nmemb, tamanyo, cmp)` | Busqueda binaria por clave. Devuelve la posicion o `NO_EXISTE`. | Toda consulta por DNI/ID (`miembro_mostrar_por_dni`, `titulo_mostrar_por_id`, alquiler, etc.). |
| `indice_vacio(idx)` / `indice_lleno(idx)` | Chequeos de cantidad. | Guardas antes de listar o insertar. |
| `indice_cargar(path, idx, vreg_ind, tamanyo, cmp)` | Carga un indice desde un binario plano (no se usa en el flujo actual de CSV, queda como utilidad de catedra). | — |
| `indice_vaciar(idx)` | Libera la memoria (`free`) y deja el indice en 0. | Se llama al final de `main()` antes de salir. |

`cmp_reg_indice` (definida en `funciones.c`) es el comparador estandar
que se le pasa a todas las funciones de indice: compara por el campo
`dni`.

---

## 4. `funciones.c` / `funciones.h` — Utilidades generales, validaciones, incidencias, alquileres y menu

### 4.1 Ordenamiento y comparacion generica

| Funcion | Que hace | Como se usa |
|---|---|---|
| `cmp_reg_indice(a, b)` | Compara dos `t_reg_indice` por `dni`. | Comparador para todas las operaciones de `t_indice`. |
| `ordenamiento_generico(base, nmemb, tamanyo, cmp)` | **Ordenamiento por inserccion generico** (no usa `qsort`): toma cualquier arreglo (`void*`), copia el elemento a insertar en un buffer temporal y lo desplaza a su posicion, igual criterio que usa `indice_insertar` pero aplicado in-place sobre arreglos comunes. | Se usa en **todos** los listados que necesitan ordenar una copia (`miembros_listar_por_plan`, `miembros_listar_morosos`, `miembros_listar_menores`, `titulos_listar_*`, etc.). Para usarla: definir una funcion `int cmp(const void*, const void*)` propia del criterio de orden, copiar los registros a ordenar a un arreglo temporal y llamar `ordenamiento_generico(copia, n, sizeof(tipo), cmp)`. |
| `igual_sin_mayus(a, b)` | Compara dos strings ignorando mayusculas/minusculas. | Usada para que el usuario pueda escribir `"basic"`, `"Basic"` o `"BASIC"` al filtrar por plan o genero. |

### 4.2 Normalizacion y CUIL

| Funcion | Que hace |
|---|---|
| `calcular_cuil(dni, sexo, cuil_str)` | Calcula el CUIL con el digito verificador real (algoritmo mod 11). |
| `normalizar_nombre(src, dest)` | Reordena/capitaliza `"Apellido, Nombre"` (separa por coma, capitaliza cada palabra). |

### 4.3 Validaciones (usadas por `miembro_validar` / `titulo_validar`)

`validar_generico(dato, validar)` ejecuta el puntero a funcion `validar`
sobre `dato` — es el mismo patron "generico via puntero a funcion" que
usa `ordenamiento_generico` y el TDA Indice.

`validar_dni`, `validar_nombre`, `validar_sexo`, `validar_plan`,
`validar_genero`, `validar_stock`, `validar_id_pelicula`, `validar_email`:
cada una chequea la regla de negocio de su campo (rango de DNI, plan
permitido, formato de email, etc.). No se llaman directamente: se
invocan vía `validar_generico` dentro de `miembro_validar`/`titulo_validar`.

### 4.4 Incidencias (errores de carga de CSV)

Cuando se carga `miembros.csv`/`titulos.csv` original (no el "fechado"),
cada fila que no pasa la validacion se registra como incidencia en vez
de descartarse silenciosamente.

| Funcion | Que hace |
|---|---|
| `incidencias_miembros_init` / `incidencias_titulos_init` | Inicializan los contadores en 0 y los nombres de cada tipo de error. |
| `incidencias_miembros_agregar(inc, tipo, dni)` / `incidencias_titulos_agregar(inc, tipo, id)` | Agregan un DNI/ID a la fila del tipo de error correspondiente. |
| `incidencias_miembros_imprimir` / `incidencias_titulos_imprimir` | Imprimen las incidencias agrupadas por tipo (con la lista de DNIs/IDs). |
| `incidencias_ordenar_miembros` / `incidencias_ordenar_titulos` | Ordenan las filas de incidencias por cantidad descendente (usa `qsort` con `cmp_fila_mbr`/`cmp_fila_tit`, ya que opera sobre un arreglo fijo de tamano `ERR_*_MAX`, no sobre los datos de negocio). |
| `incidencias_miembros_guardar_csv` / `incidencias_titulos_guardar_csv` | Persisten las incidencias a CSV al cerrar el programa. |
| `incidencias_miembros_cargar_csv` / `incidencias_titulos_cargar_csv` | Las recargan en la siguiente corrida (si se reanuda una sesion con archivos "fechados"). |
| `ver_incidencias(fp, inc_mbr, inc_tit)` | Opcion de menu **k**: muestra las incidencias actuales en pantalla. |

### 4.5 Alquileres

| Funcion | Que hace | Como se usa |
|---|---|---|
| `alquiler_buscar(arr, cant, dni, id_pelicula)` | Busqueda lineal del registro `(dni, id_pelicula)` en `arr_alquileres`. Devuelve indice o `-1`. | Uso interno de `alquiler_registrar`/`alquiler_devolver`. |
| `alquiler_activos_miembro(arr, cant, dni)` | Suma `alquileres_activos` de todas las peliculas que tiene un socio. | Usado para el limite de 2 peliculas simultaneas en plan BASIC (`LIMITE_BASIC`). |
| `alquiler_registrar(...)` | Opcion de menu **h → 1**: pide DNI e ID de pelicula, valida socio/titulo/stock/limite de plan, descuenta `stock`, crea o incrementa el registro de alquiler. | Interactiva. |
| `alquiler_devolver(...)` | Opcion de menu **h → 2**: pide DNI e ID, repone `stock`, decrementa `alquileres_activos`. | Interactiva. |
| `alquileres_guardar_csv` / `alquileres_cargar_csv` | Persisten/leen `arr_alquileres` en `alquileres_AAAAMMDD.csv`. | Automatico al abrir/cerrar el programa. |
| `alquileres_listar_indice(arr, cant, arr_mbr, idx_mbr, arr_tit, idx_tit)` | Opcion de menu **n**: construye un **indice transitorio** de alquileres ordenado por DNI (mismo TDA que `indice_miembros`), y para cada alquiler resuelve nombre del socio y titulo de la pelicula buscando por `indice_buscar` en `idx_mbr`/`idx_tit`. | No pide parametros, recorre todo `arr_alquileres`. |
| `miembros_listar_con_alquileres_activos(arr_alq, cant_alq, arr_mbr, idx_mbr)` | Opcion de menu **w**: agrupa `alquileres_activos` por DNI (puede tener varias peliculas sin devolver), ordena descendente por cantidad y muestra DNI + nombre + cantidad sin devolver. | No pide parametros. |
| `titulos_listar_mas_alquilados(arr_alq, cant_alq, arr_tit, idx_tit)` | Opcion de menu **x**: agrupa `total_alquileres` por `id_pelicula`, ordena descendente y muestra titulo + total historico. | No pide parametros. |

### 4.6 Infraestructura de archivos y menu

| Funcion | Que hace |
|---|---|
| `archivos_fechados_existen(fp, data_path)` | Indica si ya existen `miembros_AAAAMMDD.csv` y `titulos_AAAAMMDD.csv` para la fecha de proceso (sesion previa guardada). |
| `imprimir_menu()` | Imprime el menu principal completo (opciones `a` a `x`, mas `l` para salir). |

---

## 5. `miembro.c` / `miembro.h` — Gestion de socios

### 5.1 CRUD e interaccion

| Funcion | Que hace |
|---|---|
| `miembro_validar(m, fp)` | Valida todos los campos de un socio contra reglas de negocio (DNI, edad minima 10 anios, fechas coherentes entre si y con la fecha de proceso, plan valido, email de tutor obligatorio si es MENOR). Devuelve `-1` si esta OK, o el codigo `ERR_MBR_*` del primer campo invalido. |
| `miembros_cargar_csv(path, arr, cant, idx, inc, fp)` | Carga el CSV **original** (`miembros.csv`), calculando CUIL y categoria, validando cada fila y registrando incidencias en las que fallan. |
| `miembros_cargar_csv_fechado(path, arr, cant, idx)` | Carga el CSV de una sesion **ya guardada** (`miembros_AAAAMMDD.csv`), sin re-validar (los datos ya fueron validados al guardarlos). |
| `miembros_guardar_csv(path, arr, cant)` | Guarda todos los socios (activos y de baja) en CSV fechado. |
| `miembro_alta(arr, cant, idx, fp)` | Opcion de menu **a**: pide todos los datos por teclado, calcula categoria/CUIL, valida y agrega. |
| `miembro_baja(arr, idx, fp)` | Opcion de menu **c**: pide DNI, marca `estado='B'` y lo saca del indice (baja logica). |
| `miembro_modificar(arr, idx, fp)` | Opcion de menu **e**: pide DNI, muestra el socio, permite editar un campo a la vez y re-valida antes de guardar. |
| `miembro_mostrar(m)` | Imprime todos los campos de un socio (usada por varias opciones). |
| `miembro_mostrar_por_dni(arr, idx)` | Opcion de menu **g**: pide DNI, busca por indice (`indice_buscar`) y muestra el socio. |

### 5.2 Listados (todas filtran por `estado == 'A'` salvo que se indique lo contrario)

| Funcion | Opcion | Filtro | Orden | Parametros que pide |
|---|---|---|---|---|
| `miembros_listar_por_dni(arr, idx)` | **i** | todos los activos (recorre el indice) | DNI ascendente (orden natural del indice) | ninguno |
| `miembros_listar_por_plan(arr, cant)` | **j** | activos | nombre (`cmp_por_nombre`) | ninguno — muestra **todos** los planes en columnas |
| `miembros_listar_morosos(arr, cant, fp)` | **m** | activos con `fecha_diferencia_dias(ultima_cuota, fp) > 90` | fecha de ultima cuota ascendente (mas antigua primero) | ninguno (usa la fecha de proceso global) |
| `miembros_listar_menores(arr, cant)` | **o** | activos con `categoria == "MENOR"` | fecha de afiliacion ascendente | ninguno — muestra tambien `email_tutor` |
| `miembros_listar_por_plan_especifico(arr, cant, plan)` | **p** | activos con `plan` igual al pedido (sin distinguir mayusculas via `igual_sin_mayus`) | nombre | el plan (`BASIC`/`PREMIUM`/`VIP`/`FAMILY`) |
| `miembros_listar_proximos_morosidad(arr, cant, fp)` | **q** | activos con `60 <= dias_sin_pagar <= 90` | fecha de ultima cuota ascendente | ninguno |
| `miembros_listar_por_sexo(arr, cant, sexo)` | **r** | activos con `sexo` igual al pedido | DNI ascendente | el sexo (`F`/`M`/`O`) |
| `miembros_listar_antiguedad(arr, cant, anios, fp)` | **s** | activos con `calcular_edad(fecha_afiliacion, fp) > anios` (reutiliza `calcular_edad` para anios de antiguedad, no de edad biologica) | fecha de afiliacion ascendente (mas antiguo primero) | la cantidad minima de anios (`N`) |

Todas usan el mismo patron interno:
1. Copiar a un arreglo local los miembros que cumplen el filtro.
2. Si no hay ninguno, avisar y salir.
3. Llamar `ordenamiento_generico(copia, n, sizeof(t_miembro), cmp_xxx)` con el comparador adecuado (`cmp_por_nombre`, `cmp_por_fecha_cuota`, `cmp_por_fecha_afiliacion`, `cmp_por_dni`, todos `static` en `miembro.c`).
4. Imprimir la tabla con `printf` alineado.

---

## 6. `titulo.c` / `titulo.h` — Gestion de peliculas

### 6.1 CRUD e interaccion

| Funcion | Que hace |
|---|---|
| `titulo_validar(t)` | Valida ID > 0, titulo no vacio, genero permitido (`Accion/Drama/Comedia/Terror`), stock >= 0. |
| `titulos_cargar_csv(path, arr, cant, idx, inc)` | Carga `titulos.csv` original, normaliza genero/titulo, valida y registra incidencias. |
| `titulos_cargar_csv_fechado(path, arr, cant, idx)` | Carga `titulos_AAAAMMDD.csv` de una sesion guardada (sin re-validar). |
| `titulos_guardar_csv(path, arr, cant)` | Guarda todos los titulos (activos y de baja). |
| `titulo_proximo_id(arr, cant)` | Calcula el siguiente ID libre (max existente + 1), usado en alta. |
| `titulo_alta(arr, cant, idx)` | Opcion de menu **b**: asigna ID automatico, pide titulo/genero/stock, valida y agrega. |
| `titulo_baja(arr, idx)` | Opcion de menu **d**: pide ID, marca `estado='B'` y lo saca del indice. |
| `titulo_modificar(arr, idx)` | Opcion de menu **f**: pide ID, permite editar titulo, genero o stock. |
| `titulo_mostrar(t)` | Imprime todos los campos de una pelicula. |
| `titulo_mostrar_por_id(arr, idx)` | Busca por ID (via `indice_buscar`) y muestra la pelicula. |

### 6.2 Listados (todos filtran `estado == 'A'`)

| Funcion | Opcion | Filtro | Orden | Parametros que pide |
|---|---|---|---|---|
| `titulos_listar_sin_stock(arr, cant)` | **t** | `stock == 0` | titulo (alfabetico) | ninguno |
| `titulos_listar_por_genero(arr, cant, genero)` | **u** | `genero` igual al pedido (`igual_sin_mayus`) | ID ascendente | el genero |
| `titulos_listar_stock_bajo(arr, cant, n)` | **v** | `stock <= n` | stock ascendente | el stock maximo a considerar (`N`) |

Mismo patron interno que los listados de miembros: copiar, chequear
vacio, `ordenamiento_generico` con `cmp_por_titulo`/`cmp_por_id`/`cmp_por_stock`
(comparadores `static` en `titulo.c`), imprimir.

---

## 7. `main.c` — Orquestacion del programa

1. Pide la ruta de los CSV (`Docs/` por defecto) y la fecha de proceso (`pedir_fecha_proceso`).
2. Crea los indices de miembros y titulos (`indice_crear`).
3. Inicializa las incidencias (`incidencias_*_init`).
4. Si existen archivos "fechados" para esa fecha (sesion previa guardada el mismo dia de proceso), los carga tal cual (`*_cargar_csv_fechado`); si no, carga los CSV originales validando (`*_cargar_csv`).
5. Corre el **menu principal** en un `do/while` hasta que se elige `l`/`L`:

| Opcion | Accion |
|---|---|
| a | Alta de miembro |
| b | Alta de titulo |
| c | Baja de miembro |
| d | Baja de titulo |
| e | Modificacion de miembro |
| f | Modificacion de titulo |
| g | Mostrar informacion de un miembro por DNI |
| h | Alquiler (sub-menu: 1=alquilar, 2=devolver) |
| i | Listado de miembros por DNI (usa el indice) |
| j | Listado de miembros por plan (todos los planes en columnas) |
| k | Ver incidencias de carga |
| m | Listado de morosos (> 90 dias sin pagar) |
| n | Indice de alquileres (socio + titulo + totales) |
| o | Listado de miembros menores activos |
| p | Listado de miembros por plan especifico (pide el plan) |
| q | Listado de miembros proximos a la morosidad (60-90 dias) |
| r | Listado de miembros por sexo (pide F/M/O) |
| s | Antiguedad de miembros (pide N anios) |
| t | Peliculas sin stock |
| u | Peliculas por genero (pide el genero) |
| v | Peliculas con stock bajo (pide el stock maximo) |
| w | Miembros con alquileres activos (sin devolver) |
| x | Peliculas mas alquiladas (historico) |
| l | Salir |

6. Al salir, ordena las incidencias (`incidencias_ordenar_*`) y guarda
   todo en CSV fechado (`miembros_AAAAMMDD.csv`, `titulos_AAAAMMDD.csv`,
   `alquileres_AAAAMMDD.csv`, `incidencias_*_AAAAMMDD.csv`).
7. Libera los indices (`indice_vaciar`) y termina.

---

## 8. Como agregar un nuevo listado (receta rapida)

Si en la defensa piden una variante de listado, el patron es siempre el mismo:

```c
static int cmp_por_loquesea(const void *a, const void *b) {
    /* devolver <0, 0, >0 segun el criterio de orden */
}

void entidad_listar_xxx(const t_entidad *arr, int cant, /* parametros del filtro */) {
    t_entidad copia[MAX_ENTIDAD];
    int n = 0, i;

    for (i = 0; i < cant; i++)
        if (arr[i].estado == 'A' && /* condicion del filtro */)
            copia[n++] = arr[i];

    if (n == 0) { printf("No hay resultados.\n"); return; }

    ordenamiento_generico(copia, (size_t)n, sizeof(t_entidad), cmp_por_loquesea);

    /* imprimir encabezado + filas con printf alineado (%-Ns) */
}
```

Y en `main.c`, dentro del `switch`, agregar el `case` con la letra
elegida, pedir por teclado los parametros del filtro (si los hay) con
`fgets` + recorte del `\n`, y llamar a la funcion. No olvidar agregar
la linea correspondiente en `imprimir_menu()` (en `funciones.c`).
