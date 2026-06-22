#include <linux/kernel.h>       // printk
#include <linux/syscalls.h>     // SYSCALL_DEFINE
#include <linux/init.h>         // init
#include <linux/uaccess.h>      // copy_to_user / put_user
#include <linux/wait.h>         // wait_queue_head_t
#include <linux/slab.h>         // kmalloc / kfree
#include <linux/spinlock.h>     // spinlock_t
#include <linux/list.h>         // struct list_head

struct message_struct {
    size_t len;
    char *data;
    struct list_head node;
};

static struct list_head ipc_channel_queue; // cola de mensajes
static wait_queue_head_t ipc_channel_wait_queue; // cola de espera para recibir mensajes
static spinlock_t ipc_channel_spinlock; // para proteger la cola

/* 
    ipc_channel_send:
    cptr: puntero a cadena (termina en null)
*/
SYSCALL_DEFINE1(ipc_channel_send, const char __user *, cptr)
{
    struct message_struct *m;
    char *temp_buf;
    size_t len = 0;
    unsigned long flags;
    int ret = 0;

    // Calcular longitud de la cadena terminada en null
    temp_buf = (char *)kmalloc(PAGE_SIZE, GFP_KERNEL);
    if (!temp_buf)
        return -ENOMEM;

    // Copiar máximo 1 MiB
    while (len < (1U << 20)) {
        size_t to_copy = min(PAGE_SIZE, (1U << 20) - len);
        
        if (copy_from_user(temp_buf, cptr + len, to_copy)) {
            ret = -EFAULT;
            goto free_temp;
        }
        
        // Buscar null terminator
        for (size_t i = 0; i < to_copy; i++) {
            if (temp_buf[i] == '\0') {
                len += i + 1; // Incluir el null terminator
                goto found_null;
            }
        }
        len += to_copy;
    }
    
    // Mensaje demasiado largo
    ret = -E2BIG;
    goto free_temp;

found_null:

    // Verificar tamaño mensaje
    if (len == 0 || len > (1U << 20)) {
        ret = -EINVAL;
        goto free_temp;
    }

    // Crear mensaje
    m = kzalloc(sizeof(*m), GFP_KERNEL); // estructura
    if (!m) {
        ret = -ENOMEM;
        goto free_temp;
    }

    m->data = kmalloc(len, GFP_KERNEL); // buffer
    if (!m->data) {
        kfree(m);
        ret = -ENOMEM;
        goto free_temp;
    }

    // Copiar datos
    if (copy_from_user(m->data, cptr, len)) {
        kfree(m->data);
        kfree(m);
        ret = -EFAULT;
        goto free_temp;
    }
    m->len = len;

    // Poner en la cola y despertar receptores
    spin_lock_irqsave(&ipc_channel_spinlock, flags); // proteger acceso a la cola
    list_add_tail(&m->node, &ipc_channel_queue); // FIFO
    spin_unlock_irqrestore(&ipc_channel_spinlock, flags); // Despertar a los que esperan
    wake_up_interruptible(&ipc_channel_wait_queue); // Despertar a los que esperan

    printk(KERN_INFO "ipc_channel: Mensaje enviado, len=%zu\n", len);

free_temp:
    kfree(temp_buf);
    return ret;
}

/*
    ipc_channel_receive:
    cptr: puntero a buffer para recibir cadena (debe incluir null terminator)
    Retorna: número de bytes copiados (incluyendo null terminator) o error negativo
*/
SYSCALL_DEFINE1(ipc_channel_receive, char __user *, cptr)
{
    struct message_struct *m;
    unsigned long flags;
    int ret;

    // Bloquear hasta que haya mensaje
    ret = wait_event_interruptible(ipc_channel_wait_queue, !list_empty(&ipc_channel_queue));
    if (ret)
        return ret;

    // Tomar primero de la cola (FIFO)
    spin_lock_irqsave(&ipc_channel_spinlock, flags);
    if (list_empty(&ipc_channel_queue)) {
        spin_unlock_irqrestore(&ipc_channel_spinlock, flags);
        return -EAGAIN;
    }
    m = list_first_entry(&ipc_channel_queue, struct message_struct, node);

    // Obtener mensaje de la cola y desbloquear
    list_del(&m->node);
    spin_unlock_irqrestore(&ipc_channel_spinlock, flags);

    // Copiar al usuario
    if (copy_to_user(cptr, m->data, m->len)) {
        ret = -EFAULT;
        goto free_temp_receive;
    }

    printk(KERN_INFO "ipc_channel: Mensaje recibido, len=%zu\n", m->len);
    ret = m->len; // Devolver el tamaño del mensaje copiado

free_temp_receive:
    kfree(m->data); // Liberar memoria
    kfree(m);       // Liberar estructura
    return ret;
}

// Iniciar en boot
static int __init ipc_channel_init(void)
{
    INIT_LIST_HEAD(&ipc_channel_queue);
    spin_lock_init(&ipc_channel_spinlock);
    init_waitqueue_head(&ipc_channel_wait_queue);
    printk(KERN_INFO "ipc_channel: Módulo inicializado\n");
    return 0;
}
device_initcall(ipc_channel_init);