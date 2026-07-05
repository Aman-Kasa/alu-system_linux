#include "syscalls.h"

/**
 * read_string - reads a NUL-terminated string from the traced process
 * @pid: pid of the traced process
 * @addr: remote address of the string
 *
 * Return: pointer to a static buffer containing the string
 **/
char *read_string(pid_t pid, unsigned long addr)
{
	static char buf[4096];
	size_t i = 0;
	long word;

	if (addr == 0)
		return (NULL);
	while (i < sizeof(buf) - sizeof(long))
	{
		errno = 0;
		word = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
		if (word == -1 && errno != 0)
			break;
		memcpy(buf + i, &word, sizeof(long));
		if (memchr(&word, '\0', sizeof(long)) != NULL)
			break;
		i += sizeof(long);
	}
	buf[sizeof(buf) - 1] = '\0';
	return (buf);
}

/**
 * print_one_arg - prints a single argument according to its type
 * @type: the type of the argument
 * @val: the raw register value of the argument
 * @pid: pid of the traced process
 **/
void print_one_arg(type_t type, size_t val, pid_t pid)
{
	char *str;

	if (type == CHAR_P)
	{
		str = read_string(pid, val);
		if (str == NULL)
			printf("0x0");
		else
			printf("\"%s\"", str);
	}
	else if (IS_UINT(type))
		printf("%lu", (unsigned long)val);
	else if (IS_LONG(type))
		printf("%ld", (long)val);
	else if (IS_INT(type))
		printf("%d", (int)val);
	else
		printf("%#lx", val);
}

/**
 * print_args - print system call arguments
 * @sc: pointer to syscall struct
 * @regs: registers (struct user_regs_struct)
 * @pid: pid of the traced process
 **/
void print_args(const syscall_t *sc, struct user_regs_struct *regs, pid_t pid)
{
	size_t i, params[MAX_PARAMS];

	params[0] = regs->rdi, params[1] = regs->rsi, params[2] = regs->rdx;
	params[3] = regs->r10, params[4] = regs->r8, params[5] = regs->r9;

	putchar('(');
	for (i = 0; sc->params[0] != VOID && i < sc->nb_params; i++)
	{
		if (i)
			printf(", ");
		if (sc->params[i] == VARARGS)
			printf("...");
		else
			print_one_arg(sc->params[i], params[i], pid);
	}
}

/**
 * print_ret - prints the return value of a syscall according to its type
 * @sc: pointer to syscall struct
 * @regs: registers (struct user_regs_struct)
 **/
void print_ret(const syscall_t *sc, struct user_regs_struct *regs, pid_t pid)
{
	printf(") = ");
	if (sc == NULL || IS_POINTER(sc->ret))
	{
		if (sc != NULL && sc->ret == CHAR_P)
			print_one_arg(CHAR_P, regs->rax, pid);
		else
			printf("%#lx", (size_t)regs->rax);
	}
	else if (IS_UINT(sc->ret) || IS_LONG(sc->ret) || IS_INT(sc->ret))
		print_one_arg(sc->ret, regs->rax, pid);
	else
		printf("%#lx", (size_t)regs->rax);
}

/**
 * main - traces a process and prints its system calls
 * @argc: argument count
 * @argv: argument array
 * @envp: environment parameters
 * Return: 0 on success | 1 on failure (not enough arguments)
 **/
int main(int argc, char *argv[], char *envp[])
{
	int skip, status;
	size_t i, envc;
	struct user_regs_struct regs;
	pid_t pid;
	const syscall_t *sc;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <full_path> [path_args]\n", argv[0]);
		return (1);
	}
	setbuf(stdout, NULL);
	pid = fork();
	if (pid == 0)
	{
		printf("execve(\"%s\", [", argv[1]);
		for (i = 1; argv[i] != NULL; i++)
			printf("%s\"%s\"", i > 1 ? ", " : "", argv[i]);
		for (envc = 0; envp[envc] != NULL; envc++)
			;
		printf("], [/* %lu vars */]", (unsigned long)envc);
		ptrace(PTRACE_TRACEME, pid, NULL, NULL);
		execve(argv[1], argv + 1, envp);
	}
	else
	{
		for (status = 1, skip = 0; !WIFEXITED(status); skip ^= 1)
		{
			ptrace(PT_SYSCALL, pid, NULL, NULL);
			wait(&status);
			ptrace(PT_GETREGS, pid, NULL, &regs);
			if (skip)
			{
				sc = get_syscall(regs.orig_rax);
				if (sc == NULL)
					printf("\nsyscall_%llu(", regs.orig_rax);
				else
				{
					printf("\n%s", sc->name);
					print_args(sc, &regs, pid);
				}
			}
			else if (WIFEXITED(status))
				printf(") = ?\n");
			else
				print_ret(get_syscall(regs.orig_rax), &regs, pid);
		}
	}

	return (0);
}
