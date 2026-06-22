#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define SYSCALL_IPC_CHANNEL_SEND 601

int main(void) {
    char msg[256];

    while (1) {
        printf("Ingrese el mensaje a enviar (Ctrl+C para salir): ");
        if (fgets(msg, sizeof(msg), stdin) == NULL) {
            printf("\nError al leer el mensaje o fin de entrada.\n");
            break;
        }

        // Eliminar el salto de línea al final del mensaje
        msg[strcspn(msg, "\n")] = '\0';

        printf("Enviando mensaje: \"%s\"\n", msg);

        if (syscall(SYSCALL_IPC_CHANNEL_SEND, msg) < 0) {
            int e = errno;
            printf("Error al enviar el mensaje. errno=%d (%s)\n", e, strerror(e));
        } else {
            printf("Mensaje enviado: \"%s\"\n", msg);
        }
    }

    return 0;
}
