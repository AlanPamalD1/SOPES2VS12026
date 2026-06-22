#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define SYSCALL_IPC_CHANNEL_RECEIVE 602

int main(void) {
    char buffer[1024 * 1024]; // 1 MiB maximo

    int bytes_received = syscall(SYSCALL_IPC_CHANNEL_RECEIVE, buffer);
    if (bytes_received < 0) {
        int e = errno;
        printf("errno=%d (%s)\n", e, strerror(e));
        return -1;
    }

    printf("Recibido: %s (%d bytes)\n", buffer, bytes_received);
    return 0;
}