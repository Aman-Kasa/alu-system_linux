#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include "syscalls.h"

/**
 * print_child_string - Reads and prints a string from child process memory
 * @pid: Process ID of the child
 * @addr: Address of the string in the child memory space
 */
void print_child_string(pid_t pid, unsigned long addr)
{
	char buf[4096];
	size_t i = 0;
	long word;
	char *ptr;

	if (addr == 0)
	{
		printf("0");
		return;
	}
	while (i < sizeof(buf) - 1)
	{
		word = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
		if (word == -1)
			break;
		ptr = (char *)&word;
		memcpy(buf + i, ptr, 8);
		if (memchr(ptr, '\0', 8))
			break;
		i += 8;
	}
	printf("\"%s\"", buf);
}

/**
 * print_init_execve - Prints the manual string footprint for the initial execve
 * @argv: Argument vector
 * @envp: Environment variables
 */
void print_init_execve(char **argv, char **envp)
{
	int env_count = 0;
	while (envp[env_count])
		env_count++;
	printf("execve(\"%s\", [\"%s\"", argv[1], argv[1]);
	if (argv[2])
		printf(", \"%s\"", argv[2]);
	printf("], [/* %d vars */]) = 0\n", env_count);
}

/**
 * main - Traces a command, dereferencing string arguments.
 */
int main(int argc, char **argv, char **envp)
{
	pid_t pid;
	int status, is_entry = 0;
	struct user_regs_struct r;
	const char *name;

	if (argc < 2)
		return (1);
	setbuf(stdout, NULL);
	pid = fork();
	if (pid == 0)
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execve(argv[1], argv + 1, envp);
		exit(1);
	}
	wait(&status);
	ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
	print_init_execve(argv, envp);
	while (1)
	{
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		wait(&status);
		if (WIFEXITED(status))
			break;
		if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
		{
			ptrace(PTRACE_GETREGS, pid, NULL, &r);
			name = syscalls_64[r.orig_rax].name;
			if (!is_entry)
			{
				printf("%s(", name);
				if (strcmp(name, "access") == 0 || strcmp(name, "open") == 0)
				{
					print_child_string(pid, r.rdi);
					printf(", 0x%lx", (unsigned long)r.rsi);
				}
				else
				{
					int n = syscalls_64[r.orig_rax].def, i;
					unsigned long args[6];
					args[0] = r.rdi; args[1] = r.rsi; args[2] = r.rdx;
					args[3] = r.r10; args[4] = r.r8; args[5] = r.r9;
					for (i = 0; i < n; i++)
					{
						printf(args[i] == 0 ? "0" : "0x%lx", args[i]);
						if (i < n - 1) printf(", ");
					}
				}
				printf(") = ");
				is_entry = 1;
			}
			else
			{
				printf(((long)r.rax == 0) ? "0\n" : "0x%lx\n", (unsigned long)r.rax);
				is_entry = 0;
			}
		}
	}
	return (0);
}
