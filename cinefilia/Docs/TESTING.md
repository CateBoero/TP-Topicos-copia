# Cinefilia — Guia de testing manual

Esta guia explica como probar **cada opcion del menu**, que dataset
usar y que resultado esperar. Todos los resultados de las tablas ya
fueron verificados ejecutando el binario compilado.

Se agregan dos datasets de prueba pensados especificamente para esto
(ademas de los `Docs/miembros.csv` / `Docs/titulos.csv` originales de
la catedra, que siguen funcionando igual):

- **`Docs/test_listados/`** — `miembros.csv` + `titulos.csv` "originales"
  (sin fecha), pensados para probar todos los listados y las
  incidencias de carga. Se cargan con la fecha de proceso **01/12/2026**.
- **`Docs/test_alquileres/`** — sesion ya "fechada"
  (`miembros_20261215.csv`, `titulos_20261215.csv`,
  `alquileres_20261215.csv`) con datos de alquiler precargados, para
  probar el indice de alquileres y los cruces sin tener que dar de
  alta alquileres a mano. Se carga con la fecha de proceso
  **15/12/2026** (tiene que coincidir exactamente para que el programa
  detecte la sesion "fechada"; ver seccion 0).

> **Importante:** cada vez que el programa termina (`l`), reescribe
> `miembros_<fecha>.csv`, `titulos_<fecha>.csv`, etc. en la carpeta que
> se uso. Si no se hizo ningun alta/baja/alquiler durante la corrida,
> los archivos quedan iguales. Si se prueban las opciones de alta/baja
> (a, b, c, d, e, f, h) conviene **copiar la carpeta de test a una
> carpeta descartable** antes de correr el programa, para no tener que
> reconstruir el dataset:
> ```
> cp -r Docs/test_alquileres Docs_scratch
> ```
> y usar `Docs_scratch` como ruta al iniciar el programa.

---

## 0. Como arrancar el programa con cada dataset

Al ejecutar el `.exe`, el programa pide dos cosas antes de mostrar el
menu:

1. **Ruta de archivos CSV** (Enter = `Docs/`): escribir `Docs/test_listados`
   o `Docs/test_alquileres` segun lo que se quiera probar.
2. **Fecha de proceso (DD/MM/AAAA)**: tiene que ser **exactamente**
   `01/12/2026` para `test_listados` (no tiene archivos fechados, asi
   que carga el CSV original y queda en estado "recien cargado") o
   `15/12/2026` para `test_alquileres` (tiene que coincidir con el
   sufijo `_20261215` de los archivos para que el programa los
   detecte como "sesion existente" y los cargue tal cual, incluyendo
   los alquileres).

Si se ingresa cualquier otra fecha para `test_alquileres`, el programa
no va a encontrar los archivos fechados y va a intentar abrir
`miembros.csv`/`titulos.csv` (que no existen en esa carpeta), cargando
0 miembros y 0 titulos — no es un error del programa, es una carpeta
de prueba distinta a la del flujo "original".

---

## 1. Listados de miembros (CRUD ya existente)

Dataset: `Docs/test_listados`, fecha de proceso `01/12/2026`.

### 1.1 Opcion `k` — Ver incidencias de carga

El archivo `miembros.csv` de prueba tiene 10 filas: una con fecha de
nacimiento invalida, una con plan invalido y una con DNI duplicado.
El archivo `titulos.csv` tiene una fila con genero invalido y otra con
stock invalido. Asi se prueba que la carga detecta y reporta los
errores sin romper el resto de la carga.

Secuencia: `k` → en el sub-menu elegir `1` (miembros) o `2` (titulos).
**Importante:** este sub-menu pide un numero con `scanf`, no con
`fgets` como el menu principal — alcanza con escribir el numero y
Enter.

Resultado esperado (`1`):

```
DNI             | 1          | 10000001
FECHA_NAC       | 1          | 10000008
PLAN            | 1          | 10000009
```

Resultado esperado (`2`):

```
GENERO     | 1          | 9
STOCK      | 1          | 10
```

Y en la carga inicial del programa: `Miembros cargados : 7` (10 filas
- 3 invalidas) y `Titulos cargados : 8` (10 filas - 2 invalidas).

### 1.2 Opcion `i` — Listado de miembros por DNI (usa el indice)

Sin pedir nada, lista los 7 miembros activos ordenados por DNI
ascendente (orden natural del indice).

### 1.3 Opcion `j` — Listado de miembros por plan (todos los planes)

Sin pedir nada, muestra una tabla con una fila por miembro y una
columna por plan (BASIC/PREMIUM/VIP/FAMILY), ordenada por nombre.

### 1.4 Opcion `m` — Listado de morosos (> 90 dias sin pagar)

Secuencia: `m`.

Resultado esperado (ordenado ascendente por fecha de ultima cuota, el
mas antiguo primero):

```
DNI          Apellidos y Nombres            Ult. Cuota   Dias
10000006     Gomez, Luis                    01/03/2026   275
10000002     Perez, Juan                    01/06/2026   183
```

### 1.5 Opcion `q` — Proximos a la morosidad (60 a 90 dias inclusive)

Secuencia: `q`.

Resultado esperado:

```
DNI          Apellidos y Nombres            Ult. Cuota   Dias
10000004     Sosa, Pedro                    15/09/2026   77
10000005     Ruiz, Carla                    02/10/2026   60
```

Sirve para probar el limite inferior (Ruiz, exactamente 60 dias, SI
entra) y que a partir de 91 dias ya pasa a "moroso" y no a "proximo"
(ver 1.4, Perez con 183 dias no aparece aca).

### 1.6 Opcion `o` — Miembros menores activos

Secuencia: `o`.

Resultado esperado (ordenado por fecha de afiliacion ascendente,
mostrando el email del tutor):

```
DNI          Apellidos y Nombres            Afiliacion   Email Tutor
10000002     Perez, Juan                    01/01/2020   jperez.tutor@gmail.com
10000005     Ruiz, Carla                    01/01/2021   cruiz.tutor@gmail.com
```

### 1.7 Opcion `p` — Miembros por plan especifico

Secuencia: `p` → escribir el plan, por ejemplo `VIP` (no distingue
mayusculas, `vip` tambien funciona).

Resultado esperado:

```
DNI          Apellidos y Nombres            Plan
10000006     Gomez, Luis                    VIP
10000002     Perez, Juan                    VIP
```

Probar tambien con un plan sin miembros activos (ej. `FAMILY` da
"Sosa, Pedro" solamente) y con un plan que no existe (ej. `GOLD`) para
ver el mensaje `No hay miembros activos con el plan 'GOLD'.`.

### 1.8 Opcion `r` — Miembros por sexo

Secuencia: `r` → escribir `M`, `F` u `O`.

Resultado esperado para `M` (ordenado por DNI ascendente):

```
DNI          Apellidos y Nombres            Sexo
10000002     Perez, Juan                    M
10000004     Sosa, Pedro                    M
10000006     Gomez, Luis                    M
```

Para `O` deberia listar solo a Ruiz, Carla (10000005).

### 1.9 Opcion `s` — Antiguedad de miembros

Secuencia: `s` → escribir un numero de anios, por ejemplo `5`.

Resultado esperado (estrictamente **mayor** a 5 anios afiliado,
ordenado de mas antiguo a mas reciente):

```
DNI          Apellidos y Nombres            Afiliacion   Anios
10000006     Gomez, Luis                    01/01/2005   21
10000001     Lopez, Ana                     01/01/2010   16
10000004     Sosa, Pedro                    01/01/2015   11
10000007     Fernandez, Sofia               01/01/2018   8
10000002     Perez, Juan                    01/01/2020   6
```

Notar que Ruiz, Carla (afiliada el 01/01/2021, exactamente 5 anios de
antiguedad a la fecha de proceso) **no** aparece porque la condicion
es `> 5`, no `>= 5` — buen caso de prueba de limite.

---

## 2. Listados de titulos

Dataset: `Docs/test_listados`, fecha de proceso `01/12/2026`.

### 2.1 Opcion `t` — Peliculas sin stock

Secuencia: `t`.

Resultado esperado (ordenado alfabeticamente por titulo; notar que el
titulo se normaliza igual que un nombre de persona, por eso aparece
con coma):

```
ID     Titulo                         Genero
2      Titulo, Dos                    Drama
7      Titulo, Siete                  Comedia
```

### 2.2 Opcion `u` — Peliculas por genero

Secuencia: `u` → escribir el genero, por ejemplo `Accion` (no
distingue mayusculas).

Resultado esperado (ordenado por ID):

```
ID     Titulo                         Stock
1      Titulo, Uno                    5
5      Titulo, Cinco                  10
```

Probar tambien `Terror` (IDs 4 y 8) y un genero invalido como
`Anime` para ver `No hay peliculas activas del genero 'Anime'.`.

### 2.3 Opcion `v` — Peliculas con stock bajo

Secuencia: `v` → escribir el stock maximo a considerar, por ejemplo `2`.

Resultado esperado (`stock <= 2`, ordenado por stock ascendente; los
empates en stock se mantienen en el orden original del archivo,
porque `ordenamiento_generico` es un ordenamiento estable):

```
ID     Titulo                         Stock
2      Titulo, Dos                    0
7      Titulo, Siete                  0
3      Titulo, Tres                   1
8      Titulo, Ocho                   1
4      Titulo, Cuatro                 2
```

---

## 3. Indice de alquileres y cruces (miembros + titulos + alquileres)

Dataset: `Docs/test_alquileres`, fecha de proceso `15/12/2026`.

Datos de partida (`alquileres_20261215.csv`):

| DNI | Socio | ID pelicula | Titulo | Total historico | Activos (sin devolver) |
|---|---|---|---|---|---|
| 11111111 | Garcia, Pepe | 1 | El Padrino | 5 | 1 |
| 11111111 | Garcia, Pepe | 2 | Terminator | 2 | 0 |
| 22222222 | Lopez, Maria | 1 | El Padrino | 3 | 0 |
| 22222222 | Lopez, Maria | 3 | Rocky | 4 | 2 |
| 33333333 | Diaz, Carlos | 2 | Terminator | 1 | 1 |

### 3.1 Opcion `n` — Indice de alquileres

Secuencia: `n`.

Resultado esperado (las 5 filas, ordenadas por DNI, con el nombre del
socio y el titulo resueltos via `indice_buscar` sobre `indice_miembros`
e `indice_titulos`):

```
DNI          Miembro                        Titulo                         Totales  Activos
11111111     Garcia, Pepe                   El Padrino                     5        1
11111111     Garcia, Pepe                   Terminator                     2        0
22222222     Lopez, Maria                   El Padrino                     3        0
22222222     Lopez, Maria                   Rocky                          4        2
33333333     Diaz, Carlos                   Terminator                     1        1
```

### 3.2 Opcion `w` — Miembros con alquileres activos

Secuencia: `w`.

Resultado esperado (suma de `alquileres_activos` por DNI,
descendente; Garcia y Diaz empatan en 1 y se mantiene el orden de
aparicion en el archivo):

```
DNI          Miembro                        Sin devolver
22222222     Lopez, Maria                   2
11111111     Garcia, Pepe                   1
33333333     Diaz, Carlos                   1
```

### 3.3 Opcion `x` — Peliculas mas alquiladas

Secuencia: `x`.

Resultado esperado (suma de `total_alquileres` por pelicula,
descendente):

```
ID     Titulo                         Total alq.
1      El Padrino                     8
3      Rocky                          4
2      Terminator                     3
```

(El Padrino: 5 de Garcia + 3 de Lopez = 8. Terminator: 2 de Garcia + 1
de Diaz = 3.)

---

## 4. Alta, baja, modificacion y alquiler/devolucion (operaciones existentes)

Estas opciones son interactivas y modifican los datos, asi que conviene
probarlas sobre una copia descartable de `Docs/test_alquileres`:

```
cp -r Docs/test_alquileres Docs_scratch
```

y usar `Docs_scratch` como ruta, fecha de proceso `15/12/2026`.

### 4.1 Opcion `h` — Alquiler / Devolucion

Sub-menu: `1` para alquilar, `2` para devolver. Pide DNI e ID de
pelicula con `fgets` (texto libre, no hace falta numero de opcion).

- **Alquilar** (`h` → `1` → DNI `11111111` → ID pelicula `1`):
  descuenta el stock de "El Padrino" (de 3 a 2) e incrementa el
  alquiler existente de Garcia para esa pelicula (de 5/1 a 6/2).
  Mensaje esperado: `Alquiler registrado: 'El Padrino' para DNI
  11111111. Stock restante: 2`.
- **Devolver** (`h` → `2` → DNI `11111111` → ID pelicula `1`,
  inmediatamente despues de alquilar): repone el stock e incrementa el
  historico. Mensaje esperado: `Devolucion registrada. Alquileres
  historicos: 6`.
- **Caso de error**: `h` → `2` → DNI `11111111` → ID pelicula `2`
  (Garcia no tiene "Terminator" activo, ver tabla de 3.) → mensaje
  `No existe alquiler activo para ese miembro y pelicula.`.
- Plan **BASIC** tiene un limite de 2 peliculas activas
  (`LIMITE_BASIC`): para probarlo, dar de alta un miembro BASIC,
  alquilarle 2 peliculas distintas y verificar que el tercer intento
  de alquiler es rechazado.

### 4.2 Opcion `g` — Mostrar informacion de un miembro

Secuencia: `g` → DNI `11111111`. Debe mostrar todos los campos del
socio (igual formato que `miembro_mostrar`).

### 4.3 Opcion `e` — Modificacion de miembro

Secuencia: `e` → DNI `11111111` → elegir que campo modificar (numero
1-7). **Atencion:** si se elige `6` (Plan), el sub-menu de plan
tambien pide un **numero** (`1`=BASIC, `2`=PREMIUM, `3`=VIP,
`4`=FAMILY) via `scanf`, no el texto del plan. Lo mismo para `3`
(Sexo) que sí pide el caracter por `fgets` (`F`/`M`/`O`, ahi si es
texto).

Ejemplo: `e` → `11111111` → `6` → `3` (VIP) → `Miembro modificado
exitosamente.`. Verificar con `i` que el plan cambio.

### 4.4 Opcion `c` / `d` — Baja de miembro / titulo

Secuencia: `c` → DNI `33333333` → `Miembro 33333333 dado de baja.`.
Secuencia: `d` → ID `2` → `Titulo ID 2 dado de baja.`.

Verificar con `i` (miembros) o `t`/`u`/`v` (titulos, que ya no deberian
listar el dado de baja) que la baja es logica: el registro sigue
existiendo en el CSV guardado al salir con `estado='B'`, pero
desaparece de los listados que filtran por `estado == 'A'` y de las
busquedas por indice (`g`, `f`, etc. van a responder "no encontrado").

### 4.5 Opcion `a` / `b` — Alta de miembro / titulo

- `a`: pide DNI, nombre, fecha de nacimiento, sexo, fecha de
  afiliacion, fecha de ultima cuota, plan (numero 1-4) y, si la edad
  calculada es menor a 18, el email del tutor. La categoria
  (MENOR/ADULTO) se calcula sola a partir de la fecha de nacimiento y
  la fecha de proceso.
- `b`: asigna el ID automaticamente (maximo existente + 1), pide
  titulo, genero (numero 1-4) y stock.

Casos de error utiles para probar: DNI ya existente, fecha invalida
(ej. `31/02/2000`), socio con menos de 10 anios, fecha de afiliacion
posterior a la fecha de proceso.

### 4.6 Opcion `f` — Modificacion de titulo

Igual patron que `e`: `f` → ID → opcion de campo (1=Titulo,
2=Genero [pide numero 1-4], 3=Stock).

---

## 5. Resumen de datasets

| Carpeta | Contenido | Fecha de proceso a usar | Para probar |
|---|---|---|---|
| `Docs/` (originales de catedra) | `miembros.csv`, `titulos.csv` sin fechar | cualquiera (no hay sesion previa el primer dia) | Carga original + incidencias de catedra |
| `Docs/test_listados/` | `miembros.csv`, `titulos.csv` con casos de borde a medida | `01/12/2026` | Opciones `i, j, k, m, o, p, q, r, s, t, u, v` |
| `Docs/test_alquileres/` | sesion fechada con 3 miembros, 3 titulos y 5 alquileres | `15/12/2026` | Opciones `n, w, x`, y como base para probar `a, b, c, d, e, f, g, h` sobre una copia |

Si se necesita un dataset nuevo para un caso de borde puntual (por
ejemplo, un socio que paga el dia exacto del limite de 90 dias), basta
con copiar el formato de `Docs/test_listados/miembros.csv` (ver el
encabezado del archivo) y calcular la fecha exacta con:

```
date -d "<fecha de proceso>" +%s   # y restar/sumar 86400 * N dias
```

o simplemente eligiendo fechas y corriendo el programa: la opcion
correspondiente va a mostrar la columna "Dias" calculada, asi se puede
ajustar el dato hasta dar con el limite que se quiera probar.

---

## 6. Opcion `y` — Funciones extra de defensa

Submenu interno con 10 funcionalidades. Se accede desde el menu
principal con `y` y luego se elige el numero de la funcion. Cada item
de abajo indica con que dataset y fecha de proceso conviene probarlo y
que salida esperar.

### 6.1 (`y` → `1`) Cumpleanios del mes

Dataset: `Docs/test_listados/`, fecha **01/12/2026**.
Pide el mes (1-12). Filtra `estado=='A'` y `fecha_nacimiento.mes ==
mes`, ordena por dia ascendente.

Probar con **mes = 1**: todos los miembros activos del dataset nacieron
el 01/01, asi que la salida debe listar los 7 miembros activos
(`10000001..10000007`) con dia `1`. Probar con **mes = 5**: "No hay
miembros que cumplan anios en el mes 5.". Probar con **mes = 13**:
"Mes invalido.".

### 6.2 (`y` → `2`) Miembros sin alquileres

Dataset: `Docs/test_listados/` (no tiene `alquileres.csv`), fecha
**01/12/2026**. Sin alquileres cargados, todos los miembros activos
caen como "sin alquileres" -> 7 filas.

Para ver el caso contrario, usar `Docs/test_alquileres/` con fecha
**15/12/2026**. Los 3 miembros del dataset tienen al menos un alquiler
historico (`total_alquileres > 0`), por lo tanto la salida es "Todos
los miembros activos tienen al menos un alquiler.".

### 6.3 (`y` → `3`) Edad promedio por plan

Dataset: `Docs/test_listados/`, fecha **01/12/2026**.

| Plan | Miembros | Edad prom. |
|---|---|---|
| BASIC | 2 | 20.00 (Lopez 26 + Ruiz 14) |
| PREMIUM | 2 | 32.00 (Diaz 36 + Fernandez 28) |
| VIP | 2 | 31.00 (Perez 16 + Gomez 46) |
| FAMILY | 1 | 31.00 (Sosa 31) |

Verifica que `igual_sin_mayus` agrupa bien y que el conteo coincide
con la opcion `j` (listado por plan).

### 6.4 (`y` → `4`) Titulos sin alquileres

Dataset: `Docs/test_listados/`, fecha **01/12/2026**. Sin
`alquileres.csv` en esa carpeta, todos los titulos activos (IDs 1 a 8;
9 y 10 caen como incidencia por genero invalido y stock negativo) se
listan como nunca alquilados -> 8 filas.

Con `Docs/test_alquileres/` (fecha **15/12/2026**) los titulos activos
son 1 (El Padrino), 2 (Terminator) y 3 (Rocky), todos tienen
`total_alquileres > 0`, asi que la salida es "Todos los titulos
activos fueron alquilados al menos una vez.".

### 6.5 (`y` → `5`) Mayor / menor stock

Dataset: `Docs/test_listados/`, fecha **01/12/2026**.

```
Mayor stock:
  ID 5 - Titulo, Cinco (Accion) -> stock 10
Menor stock:
  ID 2 - Titulo, Dos (Drama) -> stock 0
```

(Los titulos aparecen con coma porque la carga aplica `normalizar_nombre`,
que interpreta los dos tokens como "apellido, nombre". Es comportamiento
preexistente, no de esta funcion.)

Hay empate de stock 0 entre el ID 2 y el 7; el algoritmo es estricto
(`<`), asi que se queda con el **primero encontrado**, que es el
**ID 2** (los titulos se cargan en orden de archivo).

### 6.6 (`y` → `6`) Miembro top historico

Dataset: `Docs/test_alquileres/`, fecha **15/12/2026**.
Totales por DNI: `11111111`=7 (5+2), `22222222`=7 (3+4), `33333333`=1.

Hay empate entre `11111111` y `22222222`. El algoritmo usa `>`
estricto, asi que se queda con el **primero encontrado** -> Garcia,
Pepe con 7 alquileres.

### 6.7 (`y` → `7`) Morosos con alquileres pendientes

Dataset: `Docs/test_alquileres/`, fecha **15/12/2026**.

Las tres ultimas cuotas son anteriores a 90 dias del 15/12/2026:
- 11111111 Garcia, ult cuota 01/05/2026 (228 dias) → 1 alquiler activo
- 22222222 Lopez,  ult cuota 15/05/2026 (214 dias) → 2 alquileres activos
- 33333333 Diaz,   ult cuota 10/05/2026 (219 dias) → 1 alquiler activo

Salida: tabla de 3 filas con columnas "Ult. Cuota | Dias | Sin
devolver". Verificar que coincide la columna "Dias" con la opcion `m`
(morosos) y la columna "Sin devolver" con la opcion `w`.

### 6.8 (`y` → `8`) Recaudacion estimada por plan

Dataset: `Docs/test_listados/`, fecha **01/12/2026**.

| Plan | Cuota | Miembros | Subtotal |
|---|---|---|---|
| BASIC | $3000 | 2 | $6000 |
| PREMIUM | $5000 | 2 | $10000 |
| VIP | $8000 | 2 | $16000 |
| FAMILY | $7000 | 1 | $7000 |
| **TOTAL** | | | **$39000** |

Si se cambian las cuotas (`precios[]` en `miembros_recaudacion_por_plan`)
hay que actualizar este cuadro.

### 6.9 (`y` → `9`) Busqueda por rango en el indice

Dataset: `Docs/test_listados/`, fecha **01/12/2026**.

Probar con `dni_min = 10000003`, `dni_max = 10000006`: debe listar 4
miembros (Diaz, Sosa, Ruiz, Gomez), en el mismo orden que la opcion
`i` (orden de indice por DNI).

Probar con `dni_min = 99999999`, `dni_max = 99999999`: "No hay
miembros activos en el rango [99999999, 99999999].".

Probar con `dni_min = 5`, `dni_max = 1`: "Rango invalido.".

Internamente la funcion hace **lower bound binario** sobre el indice
(no usa `indice_buscar` porque ese exige igualdad exacta) y luego
barre. Para verificar que el lower bound funciona, probar con
`dni_min = 10000004`, `dni_max = 10000005`: debe arrancar **directo en
Sosa**, no recorrer DNIs anteriores.

### 6.10 (`y` → `10`) Exportar miembros activos a CSV

Dataset: `Docs/test_listados/`, fecha **01/12/2026**.

Probar dejando el nombre por defecto (`miembros_activos.csv`): debe
informar "Exportados 7 miembros activos a 'miembros_activos.csv'." y
generar el archivo en el cwd con cabecera
`DNI;Apellidos y Nombres;Plan;Fecha Ultima Cuota` + 7 filas.

Probar con un path explicito (ej `salida.csv`) para confirmar que el
parametro se respeta. Si la ruta es invalida (carpeta inexistente)
debe responder "No se pudo abrir '<path>' para escritura.".

> **Tip:** despues de exportar, abrir el CSV generado con un editor de
> texto o `column -s';' -t < miembros_activos.csv` en bash para
> verificar el encabezado y el formato de la fecha (`DD/MM/AAAA`).
