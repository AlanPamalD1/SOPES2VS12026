#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h> // Necesario para for_each_process
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sopes 2");
MODULE_DESCRIPTION("LKM para listar procesos y sus estados para el Scheduler");

static int __init monitor_init(void) {
    struct task_struct *task;
    int count = 1;

    printk(KERN_INFO "SO2: Iniciando Monitor de Tareas del Scheduler...\n");

    // Iterar sobre todos los procesos que el Scheduler debe gestionar
    for_each_process(task) {
        // Para no inundar dmesg, imprimiremos solo los primeros 15 procesos
        if (count < 15) {
            printk(KERN_INFO "SO2 Scheduler Info -> PID: %d | Nombre: %s | Estado (__state): %u\n", 
                   task->pid, task->comm, task->__state);
            count++;
        }
    }
    printk(KERN_INFO "Mostrando %d procesos en dmesg.\n", count);
    return 0;
}

static void __exit monitor_exit(void) {
    printk(KERN_INFO "SO2: Monitor de Tareas descargado.\n");
}

module_init(monitor_init);
module_exit(monitor_exit);