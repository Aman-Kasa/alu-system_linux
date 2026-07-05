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
 * print_args - print system call arguments
 * @sc: pointer to syscall struct
 * @regs: registers (struct user_regs_struct)
 * @pid: pid of the traced process
 **/
void print_args(const syscall_t *sc, struct user_regs_struct *regs, pid_t pid)
{
	size_t i, params[MAX_PARAMS];
	char *str;

	params[0] = regs->rdi, params[1] = regs->rsi, params[2] = regs->rdx;
	params[3] = regs->r10, params[4] = regs->r8, params[5] = regs->r9;

	putchar('(');
	for (i = 0; sc->params[0] != VOID && i < sc->nb_params; i++)
	{
		if (i)
			printf(", ");
		if (sc->params[i] == VARARGS)
			printf("...");
		else if (sc->params[i] == CHAR_P)
		{
			str = read_string(pid, params[i]);
			if (str == NULL)
				printf("0x0");
			else
				printf("\"%s\"", str);
		}
		else
			printf("%#lx", params[i]);
	}
}

/**
 * print_execve_args - prints the argv/envp of an execve call before it
 *                      overwrites the traced process memory
 * @argv: the argv of the traced command (argv + 1 of this program)
 * @envp: the environment given to the traced command
 **/
void print_execve_args(char **argv, char **envp)
{
	size_t i, envc;

	printf("execve(\"%s\", [", argv[0]);
	for (i = 0; argv[i] != NULL; i++)
		printf("%s\"%s\"", i ? ", " : "", argv[i]);
	for (envc = 0; envp[envc] != NULL; envc++)
		;
	printf("], [/* %lu vars */]", (unsigned long)envc);
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
		print_execve_args(argv + 1, envp);
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
			else
			{
				if (WIFEXITED(status))
					printf(") = ?\n");
				else
					printf(") = %#lx", (size_t)regs.rax);
			}
		}
	}

	return (0);
}
