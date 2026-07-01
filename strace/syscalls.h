/* Example of what your provided syscalls.h likely looks like */
typedef struct syscall_s {
    int nb;             /* Syscall number */
    const char *name;   /* Syscall name */
    int def;            /* Number of arguments */
    /* ... types array usually goes here ... */
} syscall_t;

/* Extern array provided to you by the project */
extern syscall_t syscalls_64[];
