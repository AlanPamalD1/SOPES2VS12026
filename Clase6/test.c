#include <sys/syscall.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>


struct ram_info {
    unsigned long total;
    unsigned long used;
    unsigned long free;
    unsigned long cache;
    unsigned int mem_unit;
};

struct swap_info {
    unsigned long total;
    unsigned long used;
    unsigned long free;
    unsigned int mem_unit;
};

int main(){
    struct ram_info info;
    long ret = syscall(600, &info);
    
    if (ret == -1) {
        perror("Error al llamar la syscall");
        return EXIT_FAILURE;
    }
    
    printf("Memoria total: %lu bytes\n", info.total);
    printf("Memoria usada: %lu bytes\n", info.used);
    printf("Memoria libre: %lu bytes\n", info.free);
    printf("Memoria cache: %lu bytes\n", info.cache);
    printf("Unidad de memoria: %u bytes\n", info.mem_unit);
    
    struct swap_info sinfo;
    long rets = syscall(601, &sinfo);
    if (rets == -1) {
        perror("Error al llamar la syscall swap");
        return EXIT_FAILURE;
    }
    
    printf("Memoria swap total: %lu bytes\n", sinfo.total);
    printf("Memoria swap usada: %lu bytes\n", sinfo.used);
    printf("Memoria swap libre: %lu bytes\n", sinfo.free);
    printf("Unidad de memoria swap: %u bytes\n", sinfo.mem_unit);
    
    return EXIT_SUCCESS;
}