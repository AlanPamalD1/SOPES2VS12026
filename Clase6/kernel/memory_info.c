#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/vmstat.h>
#include <linux/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/swap.h>
#include <linux/kernel_stat.h>

/*
    get_ram_info - Obtiene estadísticas de la memoria RAM del sistema.
    @ubuf: Puntero al buffer en espacio de usuario donde se almacenarán las estadísticas.
        __user: Indica que el puntero apunta a espacio de usuario, seguridad extra
    Retorna 0 en caso de éxito o un código de error en caso de fallo.
*/

struct ram_info {
    unsigned long total;
    unsigned long used;
    unsigned long free;
    unsigned long cache;
    unsigned int mem_unit;
};

SYSCALL_DEFINE1(get_ram_info, struct ram_info __user *, ubuf) {
    struct sysinfo info;
    unsigned long cached_pages, cached, used;
    struct ram_info rinfo;

    // info memoria
    si_meminfo(&info); // datos crudos de la memoria
    cached_pages = global_node_page_state(NR_FILE_PAGES) - info.bufferram; // memoria cache en páginas
    cached = cached_pages * PAGE_SIZE; // convertir a bytes

    // si mem_unit es 0, usar PAGE_SIZE como unidad
    unsigned int mem_unit = info.mem_unit ? info.mem_unit : PAGE_SIZE; 

    // memoria usada = Total - Libre - Buffers - Caché
    used = (info.totalram - info.freeram - info.bufferram - cached / mem_unit) * mem_unit;

    // escribir en la estructura (valores en bytes)
    rinfo.total = info.totalram * mem_unit;
    rinfo.used = used;
    rinfo.free = info.freeram * mem_unit;
    rinfo.mem_unit = mem_unit;

    // copiar al espacio de usuario
    if (copy_to_user(ubuf, &rinfo, sizeof(rinfo))) { // verifica que el puntero es valido y tiene permisos de escritura
        return -EFAULT;
    }

    // Imprimir en el kernel log para ver los valores
    printk(
        KERN_INFO
        "Syscall sopes2 (información memoria): total=%lu, utilizado=%lu, libre=%lu",
        rinfo.total, rinfo.used, rinfo.free
    );

    return 0;
}

/*
    get_swap_info - Obtiene estadísticas de la memoria swap del sistema.
    @ubuf: Puntero al buffer en espacio de usuario donde se almacenarán las estadísticas.
        __user: Indica que el puntero apunta a espacio de usuario, seguridad extra
    Retorna 0 en caso de éxito o un código de error en caso de fallo.
*/

struct swap_info {
    unsigned long total;
    unsigned long used;
    unsigned long free;
    unsigned int mem_unit;
};

SYSCALL_DEFINE1(get_swap_info, struct swap_info __user *, ubuf) {
    struct sysinfo info;
    unsigned long swap;
    struct swap_info sinfo;

    si_swapinfo(&info); // info swap
    swap = info.totalswap - info.freeswap; // swap usado

    unsigned int mem_unit = info.mem_unit ? info.mem_unit : PAGE_SIZE;

    // escribir en la estructura (en bytes)
    sinfo.total = info.totalswap;
    sinfo.used = swap;
    sinfo.free = info.freeswap ;
    sinfo.mem_unit = mem_unit;
    
    // copiar al espacio de usuario
    if (copy_to_user(ubuf, &sinfo, sizeof(sinfo))) {
        return -EFAULT;
    }

    printk(KERN_INFO "get_swap_info: total=%lu, used=%lu, free=%lu\n",
           sinfo.total, sinfo.used, sinfo.free);
    return 0;
}