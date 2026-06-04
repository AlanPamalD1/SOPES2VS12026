# Creación modulo teclado virtual

## Instrucciones para ejecutar el módulo de kernel

0. Instala las herramientas de compilacion
   ```bash
   sudo apt -y install build-essential libncurses-dev bison flex libssl-dev libelf-dev
   ```

1. Abre una terminal y navega al directorio del módulo:
    ```bash
    cd ~/Clase2
    ```

3. Compila el módulo usando `make`:
    ```bash
    make
    ```

4. Inserta el módulo en el kernel:
    ```bash
    sudo insmod vkeyboard.ko
    ```

5. Verifica que el módulo esté cargado:
    ```bash
    lsmod | grep vkeyboard
    ```

6. Consulta los mensajes del kernel para ver la salida del módulo:
    ```bash
    dmesg | tail
    ```

    Para presionar una tecla, escribe en /proc/virtual_keyboard_control
    ```bash
    echo "10" > /proc/virtual_keyboard_control
    ```
    
    Presiona la tecla "9" que tiene el keycode 10 

7. Para remover el módulo:
    ```bash
    sudo rmmod vkeyboard.ko
    ```

8. Limpia los archivos generados:
    ```bash
    make clean
    ```

## Instrucciones para ejecutar el programa de prueba

1. Abre una terminal y navega al directorio del programa de prueba:
    ```bash
    cd ~/Clase2
    ```

2. Compila el programa de prueba:
    ```bash
    gcc test.c -o test
    ```

3. Ejecuta el programa de prueba:
    ```bash
    ./test
    ```

* Nota: Asegúrate de que el módulo de kernel esté cargado antes de ejecutar el programa de prueba, ya que este programa interactúa con el módulo para simular la presión de teclas.

## ¿Qué hace este módulo?

Este módulo de kernel es un ejemplo básico que, al ser cargado, permite controlar un teclado virtual, escribiendo en **/proc/virtual_keyboard_control** el keycode de la tecla que se desea presionar. El módulo reporta al sistema la presion y liberacion de la tecla, y muestra un mensaje en el log del kernel cada vez que se presiona una tecla.

## Enlaces de interés

* https://linux.die.net/lkmpg/x769.html
* https://www.kernel.org/doc/html/v4.18/input/event-codes.html#event-codes