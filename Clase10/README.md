# Gestión de Memoria y Planificacion

## Gestión de Memoria

### Ejemplos:

Demuestra las funciones fundamentales de asignación y liberación de memoria:
- `malloc()` - Asignación dinámica
- `calloc()` - Asignación e inicialización a cero
- `realloc()` - Redimensionamiento de memoria
- Detección de fugas de memoria

---

### Compilación y Ejecución

* 1. Compilarlo:

```bash
gcc -Wall -Wextra -O2 -std=c99 gestion_memoria.c -o gestion_memoria
```

* 2. Ejecutar el programa:

```bash
./gestion_memoria
```

* 3. Ejecutar con Valgrind para detectar fugas de memoria:
```bash
valgrind --leak-check=full ./gestion_memoria
```

---

### Requisitos

- GCC
- valgrind (para detectar fugas de memoria)

```bash
sudo apt-get install gcc valgrind
sudo apt-get install gcc valgrind leak
```

## Fallo de Paginas

### Ejemplo:

Demuestra cómo el sistema operativo maneja los fallos de página, mostrando la asignación de memoria y el proceso de paginación.

### Compilación y Ejecución

* 1. Compilarlo:

```bash
gcc fallo_pagina.c -o fallo_pagina
```

* 2. Ejecutar el programa:

```bash
./fallo_pagina
```

## Informacion procesos

### Ejemplo:

Modulo del kernel que muestra información sobre los procesos en ejecución, como el PID y el estado.

### Compilación y Ejecución

* 1. Compilar el módulo del kernel:

```bash
cd monitor
make
```

* 2. Cargar el módulo en el kernel:

```bash
sudo insmod monitor_procesos.ko
```

* 3. Ver los mensajes del kernel para verificar la información de los procesos:

```bash
dmesg | tail
```

* 4. Descargar el módulo del kernel:

```bash
sudo rmmod monitor_procesos
```