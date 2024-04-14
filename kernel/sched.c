#include <kernel/sched.h>

#include <kernel/types.h>
#include <kernel/string.h>
#include <kernel/mm.h>

// 64 is enough for now
#define MAX_TASKS 64

static struct task proc_table[MAX_TASKS];

void init_tasking()
{
    for (size_t i = 0; i < MAX_TASKS; ++i) {
        memset(&proc_table[i], 0, sizeof(struct task));
    }
}

/* Bring one of the available task flesh to life (no need to preallocate memory) */
pid_t init_task(struct task *proc)
{
    for (size_t i = 0; i < MAX_TASKS; ++i) {
        if (proc_table[i].pid != 0)
            continue;

        proc = &proc_table[i];
        proc->pid = i;

        proc->state = EMBRYO;

        return proc->pid;
    }

    return -1; // FIXME: Error code here (Max taskes)
}

/* Free task struct memory and memory it owns */
int free_task(struct task *proc)
{
    if (proc->pid == 0) {
        return -1; // FIXME: Come up with an error code (Can't Kill the Dead)
    }

    free_address_space(proc->address_space);

    memset(proc, 0, sizeof(*proc));

    return 0;
}
