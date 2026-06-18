#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
/* Infraestructura /proc y seq_file para exponer contenido dinámico */
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
/* APIs de memoria y scheduler para obtener RSS, páginas y procesos */
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/sched/mm.h>
#include <linux/mmzone.h>
#include <linux/vmstat.h>
#include <linux/hugetlb.h>
#include <linux/mman.h>
#include <linux/sort.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sopes 2");
MODULE_DESCRIPTION("Monitor de memoria del sistema con top 10 procesos");
MODULE_VERSION("1.0");

/* Nombre de la entrada /proc */
#define PROC_FILENAME   "proc_monitor"
#define TOP_N           10

/* ------------------------------------------------------------------ */
/* Estructura para guardar info de un proceso                          */
/* ------------------------------------------------------------------ */
struct proc_mem_info {
    pid_t  pid;
    char   name[TASK_COMM_LEN];
    unsigned long rss_kb;   /* RSS en KB */
};

/* ------------------------------------------------------------------ */
/* Comparador descendente para sort()                                   */
/* ------------------------------------------------------------------ */
static int cmp_desc(const void *a, const void *b)
{
    const struct proc_mem_info *pa = a;
    const struct proc_mem_info *pb = b;

    /* sort() espera: >0 si "a" va después, <0 si "a" va antes */
    if (pb->rss_kb > pa->rss_kb) return  1;
    if (pb->rss_kb < pa->rss_kb) return -1;
    return 0;
}

/* ------------------------------------------------------------------ */
/* Recopila RSS de todos los procesos y devuelve array ordenado        */
/* Retorna número de entradas válidas; el caller debe kfree(arr)       */
/* ------------------------------------------------------------------ */
static int collect_top(struct proc_mem_info **out, int maxn)
{
    struct task_struct  *task;
    struct proc_mem_info *arr;
    int n = 0, capacity = 256;

    /* Reserva inicial; se amplía si hay más procesos */
    arr = kmalloc_array(capacity, sizeof(*arr), GFP_KERNEL);
    if (!arr)
        return -ENOMEM;

    /*
     * Recorremos la lista global de procesos bajo lock RCU.
     * for_each_process() es seguro con rcu_read_lock().
     */
    rcu_read_lock();
    for_each_process(task) {
        struct mm_struct *mm;
        unsigned long rss = 0;

        /* Saltar procesos de kernel (sin mm) */
        mm = get_task_mm(task);
        if (!mm)
            continue;

        /* get_mm_rss() retorna páginas; convertimos a KB */
        rss = get_mm_rss(mm) << (PAGE_SHIFT - 10); /* páginas -> KB */
        mmput(mm);

        if (n >= capacity) {
            /* Ampliar buffer dinámicamente */
            struct proc_mem_info *tmp;
            capacity *= 2;
            /* GFP_ATOMIC evita dormir dentro de sección crítica */
            tmp = krealloc(arr, capacity * sizeof(*arr), GFP_ATOMIC);
            if (!tmp) {
                /* Sin memoria: devolver lo que tenemos */
                break;
            }
            arr = tmp;
        }

        /* Copia de snapshot del proceso actual */
        arr[n].pid    = task->pid;
        arr[n].rss_kb = rss;
        get_task_comm(arr[n].name, task);
        n++;
    }
    rcu_read_unlock();

    /* Orden descendente por consumo de memoria */
    sort(arr, n, sizeof(*arr), cmp_desc, NULL);

    *out = arr;
    return (n < maxn) ? n : maxn;
}

/* ------------------------------------------------------------------ */
/* Escritura compacta en /proc/mem_monitor para backend                */
/* ------------------------------------------------------------------ */
static int mem_monitor_show(struct seq_file *m, void *v)
{
    struct proc_mem_info *procs = NULL;
    int i, n;

    /* Formato estable para máquina */
    n = collect_top(&procs, TOP_N);

    if (n < 0) {
        seq_puts(m, "top_count=0\n");
        kfree(procs);
        return 0;
    }

    seq_printf(m, "top_count=%d\n", n);
    for (i = 0; i < n; i++) {
        seq_printf(m, "proc=%d|%s|%lu\n",
                   procs[i].pid,
                   procs[i].name,
                   procs[i].rss_kb);
    }

    kfree(procs);
    return 0;
}

/* ------------------------------------------------------------------ */
/* open() para seq_file de una sola pasada                             */
/* ------------------------------------------------------------------ */
static int mem_monitor_open(struct inode *inode, struct file *file)
{
    /* single_open: genera contenido en una sola pasada al hacer read() */
    return single_open(file, mem_monitor_show, NULL);
}

/* ------------------------------------------------------------------ */
/* Operaciones de archivo para /proc                                   */
/* ------------------------------------------------------------------ */
static const struct proc_ops mem_monitor_fops = {
    /* seq_file helpers para open/read/lseek/release */
    .proc_open    = mem_monitor_open,
    .proc_read    = seq_read, // lee el contenido generado por mem_monitor_show()
    .proc_lseek   = seq_lseek, // permite seek dentro del contenido generado
    .proc_release = single_release, // libera recursos al cerrar el archivo
};

/* ------------------------------------------------------------------ */
/* Inicialización del módulo                                           */
/* ------------------------------------------------------------------ */
static int __init mem_monitor_init(void)
{
    struct proc_dir_entry *entry;

    /* Única entrada para backend/parsers */
    entry = proc_create(PROC_FILENAME, 0444, NULL, &mem_monitor_fops);
    if (!entry) {
        pr_err("mem_monitor: no se pudo crear /proc/%s\n", PROC_FILENAME);
        return -ENOMEM;
    }

    pr_info("mem_monitor: modulo cargado -> /proc/%s\n", PROC_FILENAME);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Limpieza del módulo                                                  */
/* ------------------------------------------------------------------ */
static void __exit mem_monitor_exit(void)
{
    /* Remover entrada /proc */
    remove_proc_entry(PROC_FILENAME, NULL);
    pr_info("mem_monitor: modulo descargado\n");
}

module_init(mem_monitor_init);
module_exit(mem_monitor_exit);