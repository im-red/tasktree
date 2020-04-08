# Tasktree

Visualize the linux LWP tree launched since system booting.

```
[0] idle(living)
 \_ [1] swapper/0 -> [1] init(living)
 |                    \_ [129] init
 |                    |   \_ [130] init -> [130] rc
 |                    |   |                 \_ [133] rc -> [133] stty
 |                    |   |                 \_ [134] rc -> [134] dmesg
 |                    |   |                 \_ [135] rc
 |                    |   |                 |   \_ [136] rc -> [136] ls
 |                    |   |                 \_ [137] rc -> [137] S00mountvirtfs
 |                    |   |                 |               \_ [138] S00mountvirtfs -> [138] stty
 |                    |   |                 |               \_ [139] S00mountvirtfs -> [139] mountpoint
 |                    |   |                 |               \_ [140] S00mountvirtfs -> [140] mount
 |                    |   |                 |               \_ [141] S00mountvirtfs -> [141] mkdir
 |                    |   |                 |               \_ [142] S00mountvirtfs -> [142] chmod
 |                    |   |                 |               \_ [143] S00mountvirtfs -> [143] echo
 |                    |   |                 |               \_ [144] S00mountvirtfs
 |                    |   |                 |               |   \_ [145] S00mountvirtfs
 |                    |   |                 |               |   \_ [146] S00mountvirtfs -> [146] sed
 |                    |   |                 |               \_ [147] S00mountvirtfs
 |                    |   |                 |               |   \_ [148] S00mountvirtfs -> [148] date
 |                    |   |                 |               |   \_ [149] S00mountvirtfs -> [149] hostname
 |                    |   |                 |               \_ [150] S00mountvirtfs -> [150] echo
 |                    |   |                 |               \_ [151] S00mountvirtfs -> [151] mountpoint
 |                    |   |                 |               \_ [152] S00mountvirtfs -> [152] echo
 |                    |   |                 |               \_ [153] S00mountvirtfs
 |                    |   |                 |               |   \_ [154] S00mountvirtfs
 |                    |   |                 |               |   \_ [155] S00mountvirtfs -> [155] sed
 |                    |   |                 |               \_ [156] S00mountvirtfs -> [156] echo
 |                    |   |                 |               \_ [157] S00mountvirtfs -> [157] mount
 |                    |   |                 |               \_ [158] S00mountvirtfs -> [158] mountpoint
 |                    |   |                 |               \_ [159] S00mountvirtfs -> [159] echo
 |                    |   |                 |               \_ [160] S00mountvirtfs
 |                    |   |                 |               |   \_ [161] S00mountvirtfs
...
```

## How to use?

1. Modify the kernel to log all `fork`/`exec`/`exit` calling to /var/log/kern.log. (We don't distinguish between threads and processes. Because there is only LWP in kernel. `_do_fork` is called to copy LWP. So we use `fork` here.)
2. Copy the log recorded since system booting to a separating log file.
3. Use this tool to parse the log.

## How to modify kernel?

For example, in linux-5.2.8, we need to modify 3 files: kernel/fork.c, fs/exec.c, kernel/exit.c

### kernel/fork.c

In function `copy_process`, we need to insert a `printk` line above `return p;`

```c
...
    trace_task_newtask(p, clone_flags);
    uprobe_copy_process(p, clone_flags);
    printk(KERN_ERR "FORK|%d|%s|=>|%d", current->pid, current->comm, p->pid); // Inserted here!
    return p;
...
```

### fs/exec.c

In function `__set_task_comm`, we need to insert a `printk` line in the beginning.

```c
void __set_task_comm(struct task_struct *tsk, const char *buf, bool exec)
{       
    printk(KERN_ERR "EXEC|%d|%s|=|%s", tsk->pid, tsk->comm, buf); // Inserted here!
    task_lock(tsk);
    trace_task_rename(tsk, buf);
    strlcpy(tsk->comm, buf, sizeof(tsk->comm));
    task_unlock(tsk);
    perf_event_comm(tsk, exec);
}
```

### kernel/exit.c

In function `do_exit`, we need to insert a `printk` line in the beginning.

```c
void __noreturn do_exit(long code)
{       
    printk(KERN_ERR "EXIT|%d|%s", current->pid, current->comm); // Inserted here!
    struct task_struct *tsk = current;
    int group_dead;

    profile_task_exit(tsk);
...
```

The modified kernel will produce log like following:

```
[    0.067912] FORK|7|kworker/u8:0|=>|43
[    0.068754] EXIT|43|kworker/u8:0
[    0.069761] FORK|2|kthreadd|=>|44
[    0.069763] EXEC|44|kthreadd|=|cryptomgr_test
[    0.070101] EXIT|44|cryptomgr_test
[    0.070778] FORK|7|kworker/u8:0|=>|45
[    0.071254] EXIT|45|kworker/u8:0
[    0.071784] FORK|2|kthreadd|=>|46
[    0.071966] EXEC|46|kthreadd|=|cryptomgr_test
[    0.072784] EXIT|46|cryptomgr_test
[    0.072798] FORK|7|kworker/u8:0|=>|47
[    0.073820] EXIT|47|kworker/u8:0
[    0.074777] FORK|2|kthreadd|=>|48
[    0.074931] EXEC|48|kthreadd|=|cryptomgr_test
[    0.075758] EXIT|48|cryptomgr_test
[    0.075762] FORK|7|kworker/u8:0|=>|49
[    0.076212] EXIT|49|kworker/u8:0
[    0.076756] FORK|2|kthreadd|=>|50
[    0.076922] EXEC|50|kthreadd|=|cryptomgr_test
[    0.077086] EXIT|50|cryptomgr_test
[    0.077763] FORK|7|kworker/u8:0|=>|51
[    0.078747] EXIT|51|kworker/u8:0
```

And this tool will produce text output like following:

```
[0] idle(living)
 \_ [1] swapper/0 -> [1] init(living)
 |                    \_ [129] init
 |                    |   \_ [130] init -> [130] rc
 |                    |   |                 \_ [133] rc -> [133] stty
 |                    |   |                 \_ [134] rc -> [134] dmesg
 |                    |   |                 \_ [135] rc
 |                    |   |                 |   \_ [136] rc -> [136] ls
 |                    |   |                 \_ [137] rc -> [137] S00mountvirtfs
 |                    |   |                 |               \_ [138] S00mountvirtfs -> [138] stty
 ```

`" -> "` means an `exec` calling. 

`" \_ "` means a `fork` calling.

The is similar with `ps(1)`/`pstree(1)`. But `ps(1)`/`pstree(1)` can only output the living LWP.