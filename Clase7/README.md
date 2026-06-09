# Ejecucion paralela con hilos

## Compilacion

```bash
gcc -o multithreading1.c -o multithreading1 -lpthread
gcc -o multithreading2.c -o multithreading2 -lpthread
gcc -o multithreading3.c -o multithreading3 -lpthread
gcc -o multithreading4.c -o multithreading4 -lpthread
gcc -o multithreading4_solucion.c -o multithreading5 -lpthread
```

## Ejecucion

```bash
./multithreading1
./multithreading2
./multithreading3
./multithreading4
./multithreading5
```

## Multithreading 1 (sin sincronizacion)

Aunque en tu código de main llamas a pthread_create para el Hilo 1 antes que para el Hilo 
2, el sistema operativo es quien decide cuándo se ejecutan realmente. 

* Al crear un hilo, este pasa al estado "Listo para ejecutarse". 
* En ese estado, el algoritmo de planificación del kernel decide cuál de todos los hilos 
"listos" toma la CPU primero. 
* A veces, el Hilo 2 recibe recursos del CPU una fracción de milisegundo antes que el 
Hilo 1, simplemente porque el planificador así lo determinó en ese momento.

## Multithreading 2 (Creación en Masa y Memoria Dinámica)

En este código, se crean 5 hilos, cada uno con un identificador único.

## Multithreading 3 Pasando Datos Complejos (Structs)

En este código, se define una estructura `args` que contiene un identificador de hilo y un mensaje.

## Multithreading 4 (Race Condition)

En este código, se incrementa una variable global `counter` sin ningún mecanismo de sincronización, lo que puede llevar a una condición de carrera.


## Multithreading 5 (Solución a Race Condition)

Con el uso de mutex, se asegura que solo un hilo pueda acceder a la sección crítica del código (donde se incrementa el contador) a la vez, evitando así la condición de carrera y garantizando que el resultado final sea correcto.