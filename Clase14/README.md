# Nueva llamada al sistema (syscall)

## Compilar el kernel con la nueva syscall

1. Guarda todos los cambios en los archivos modificados (`syscall_64.tbl`, `ipc_channel.c`, `Makefile`).

2. Desde la raíz del código fuente del kernel, compilar el kernel con el comando:
   ```bash
   fakeroot make -j$(nproc)
   ```

3. Instalar el kernel y los módulos:
   ```bash
   sudo make modules_install
   sudo make install
   ```

4. Reinicia y selecciona el nuevo kernel en el menú de arranque:
   ```bash
   sudo reboot
   ```
---

##  Compilar y ejecutar el programa

1. Compila `ipc_send.c` y  `ipc_receive.c` con el siguiente comando:
   ```bash
   gcc ipc_send.c -o ipc_send
   gcc ipc_receive.c -o ipc_receive
   ```

2. Ejecuta el programa:

   ```bash
   ./ipc_send
   ./ipc_receive
   ```

   * Se debera de ingresar un mensaje para encolarlo en la cola de mensajes. Luego, el programa `ipc_receive` se encargará de recibir el mensaje y mostrarlo en la consola.