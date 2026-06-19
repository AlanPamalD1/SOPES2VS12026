# Protección y Seguridad con PAM

## Ejecución pruebas PAM con manejo de roles

### Crear usuarios y roles

```bash
# Crear un nuevo usuario
sudo useradd -m usuario1
sudo useradd -m usuario2

# Establecer una contraseña para el nuevo usuario
sudo passwd usuario1
sudo passwd usuario2

# Crear un nuevo grupo (rol)
sudo groupadd rol_desarrollador
sudo groupadd rol_administrador

# Asignar el usuario al grupo (rol)
sudo usermod -aG rol_desarrollador usuario1
sudo usermod -aG rol_administrador usuario2 

# Verificar la asignación del usuario al grupo
groups usuario1
groups usuario2
```

* Comando para ver el usuario actual: `who am i`
* Comando para ver el UID del usuario actual: `id -u`
* Comando para ver el grupo del usuario actual: `id -Gn`
* Comando para ver el UID y GID de un usuario específico: `id <nombre_usuario>`
* Comando para ver el GID de un grupo específico: `getent group <nombre_grupo>`

### Cambio de usuario

```bash
# Cambiar a otro usuario
su - usuario1
su - usuario2

# Verificar el cambio de usuario
who am i
id -u
id -Gn
```

### Ejecución pruebas

* Compilación:


```bash
# Instalar las bibliotecas de desarrollo PAM
sudo apt install libpam0g-dev -y

# Compilar el programa de prueba PAM
gcc -Wall -Wextra -o test_rol test_rol.c pam.c -lpam -lpam_misc
```

* Ejecución:

```bash
# Ejecutar el programa de prueba PAM
cd pam

./test_rol usuario1 usuario1 rol_desarrollador
./test_rol usuario2 usuario2 rol_administrador

# Error de autenticación
./test_rol usuario1 password rol_desarrollador

# Error de grupo
./test_rol usuario1 usuario1 rol_administrador
```

---

## Manejo usuarios con servicio propio

* Crear un nuevo usuario sin shell ni home (para servicio):

```bash
nano /etc/pam.d/mi_app
```

Contenido de `/etc/pam.d/mi_app`:

```text
# /etc/pam.d/mi_app
auth     required  pam_unix.so # Autenticación con contraseña
account  required  pam_unix.so # Verificación de cuenta (expiración, bloqueo, etc.)
session  required  pam_unix.so # Gestión de sesión (registro de inicio/cierre, etc.)
```

### Compilación y ejecución del servicio

```bash
# Compilar el servicio
cd pam
gcc -Wall -Wextra -o test_completo test_completo.c pam.c -lpam -lpam_misc

# Ejecutar el servicio
./test_completo <usuario> <contraseña>
```

---

## Manejo ACLs

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
