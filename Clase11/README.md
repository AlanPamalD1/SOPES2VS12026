## Manejo de task_struct

Este modulo de kernel muestra operaciones comunes sobre `task_struct`:

- Inspeccionar el proceso actual (`current`)
- Inspeccionar el padre de un proceso (`real_parent`)
- Buscar un proceso por PID (`find_vpid` + `pid_task`)
- Recorrer la lista global de procesos (`for_each_process`)
- Recorrer el grupo de hilos de un proceso (`for_each_thread`)

### Archivos

- `task.c`: modulo LKM
- `Makefile`: compilacion del modulo

### Compilar

```bash
cd ./Clase11/modulo
make
```

### Cargar modulo

```bash
sudo insmod task.ko
```

Con parametros:

```bash
sudo insmod task.ko target_pid=1 max_tasks=15 show_threads=1
```

### Ver logs del kernel

```bash
dmesg | tail -n 100
```

Filtrar solo el modulo:

```bash
dmesg | grep task_lkm
```

### Descargar modulo

```bash
sudo rmmod task
```

### Limpiar compilacion

```bash
make clean
```

### Parametros disponibles

- `target_pid` (int): PID objetivo para inspeccionar. `-1` para omitir.
- `max_tasks` (int): cantidad maxima de procesos a listar.
- `show_threads` (bool): `1` para listar hilos del PID objetivo.

### Ejemplo de flujo de prueba

1. Obtener un PID de usuario:
```bash
pgrep -n bash
```

2. Cargar modulo con ese PID:
```bash
sudo insmod task.ko target_pid=<PID> max_tasks=10 show_threads=1
```

3. Revisar salida:
```bash
dmesg | grep task_lkm
```

4. Descargar modulo:
```bash
sudo rmmod task
```

## RSS vs VSZ