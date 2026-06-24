# gRPC con Python para la comunicación entre un cliente y un servidor.

## Instalación de gRPC y python

* Instalar gRPC y herramientas necesarias para Python:

```bash
sudo apt update
sudo apt install python3-pip

# Crear un entorno virtual para el proyecto
python3  -m pip venv venv
source venv/bin/activate

pip install -r requirements.txt
```

* Generar el código de gRPC a partir del archivo .proto:

```bash
python -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. sensor.proto
```

* Ejecutar el servidor gRPC:

```bash
python server.py
```

* Ejecutar el cliente gRPC:

```bash
python client.py
```

## Ventajas de gRPC

1. Python se encarga de la serialización y deserialización de los mensajes, lo que simplifica el desarrollo.
2. gRPC utiliza HTTP/2, lo que permite una comunicación eficiente y de baja latencia entre el cliente y el servidor.
3. Python soporta asyncio nativamente para gRPC, lo que permite manejar miles de conexiones simultáneas de forma eficiente sin complicar el código.
4. Es ideal para prototipos de Sistemas Distribuidos o microservicios donde el tiempo de desarrollo es crítico.

---

# Manejo ACLs

Este laboratorio muestra por que un modelo de permisos basado solo en ACL tradicionales no es suficiente para ciertas operaciones privilegiadas, y como Linux Capabilities permite asignar privilegios minimos y especificos.

## Objetivo

Demostrar el caso de uso de `CAP_NET_RAW` para crear un raw socket ICMP sin ejecutar todo el programa como `root`.

## Archivos

* Compilar el programa:

```bash
make
```

Ejecutar como usuario normal:

```bash
cd acl

./raw_socket_demo
```

Resultado esperado:

- Error `Operation not permitted` (o equivalente).

Esto sucede porque crear `SOCK_RAW` para ICMP requiere privilegio de kernel (`CAP_NET_RAW`).

* *2*. Por que ACL no alcanza aqui

Las ACL de archivos controlan cosas como leer/escribir/ejecutar archivos o directorios.  
Pero crear raw sockets no es un permiso de archivo: es una operacion privilegiada del kernel sobre la red.

Por eso, aunque tengas permisos de ejecucion sobre el binario, el kernel igual puede negar la operacion.

* *3*. Solucion arcaica (peligrosa)

Ejecutar con privilegios totales:

```bash
sudo ./acl/raw_socket_demo
```

Funciona, pero viola el Principio de Menor Privilegio: el proceso hereda poder total de `root`, no solo lo necesario para abrir raw sockets.

* *4*. Solucion moderna con capacidades

Asignar solo la capacidad necesaria al binario:

```bash
sudo setcap cap_net_raw+ep ./acl/raw_socket_demo
```

Verificar capacidades:

```bash
getcap ./acl/raw_socket_demo
```

Salida esperada (aprox):

```text
./acl/raw_socket_demo cap_net_raw=ep
```

* *5*. Exito sin sudo

Ejecutar nuevamente como usuario normal:

```bash
./acl/raw_socket_demo
```

Ahora debe mostrar mensaje de exito al crear el raw socket.

* *6*. Limpieza y comprobacion final

Quitar la capacidad:

```bash
sudo setcap -r ./acl/raw_socket_demo
```

Volver a ejecutar sin sudo para confirmar que vuelve a fallar.
