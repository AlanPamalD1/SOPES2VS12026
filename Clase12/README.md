# Conexion WebSocket con LKM

## Compilación y Ejecución

```bash
# Compilar el módulo del kernel
cd modulo
make

# Cargar el módulo en el kernel
sudo insmod proc_monitor.ko

# Verificar que el módulo se ha cargado correctamente
lsmod | grep proc_monitor

# Descargar el módulo del kernel
sudo rmmod proc_monitor
```

```bash
# Compilar el cliente WebSocket
cd backend
make

# Ejecutar el servidor WebSocket
./mem_ws_backend
```

## Ejecución

Abrir el archivo `index.html` en un navegador web para iniciar la conexión WebSocket con el servidor.

## Notas

* Asegúrate de que el servidor WebSocket esté en ejecución antes de abrir el archivo `index.html` para evitar errores de conexión.
* El frontend utiliza localhost y el puerto 8080 para conectarse al servidor WebSocket, asegúrate de correr el servidor en la misma máquina y puerto para que funcione correctamente, en caso contrario cambiarlo en el código del frontend (`index.html`).