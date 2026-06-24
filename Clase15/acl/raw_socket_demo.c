#include <errno.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (sockfd < 0) {
        fprintf(stderr,
                "[ERROR] No se pudo crear raw socket ICMP. errno=%d (%s)\n",
                errno,
                strerror(errno));
        fprintf(stderr,
                "[PISTA] Si ves 'Operation not permitted', falta CAP_NET_RAW.\n");
        return 1;
    }

    printf("[OK] Raw socket creado correctamente (fd=%d).\n", sockfd);
    printf("[INFO] Tu proceso tiene privilegios suficientes para IPPROTO_ICMP.\n");

    close(sockfd);
    return 0;
}
