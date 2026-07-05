#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include "syscalls.h"

/**
 * print_args - Prints the raw hex arguments for a system call
 * @r: The registers struct
 * @nb_args: Number of arguments to print
 */
void print_args(struct user_regs_struct *r, int nb_args)
{
	int i;
	unsigned long arg;

	for (i = 0; i < nb_args; i++)
	{
		switch (i)
		{
			case 0: arg = r->rdi; break;
			case 1: arg = r->rsi; break;
			case 2: arg = r->rdx; break;
			case 3: arg = r->r10; break;
			case 4: arg = r->r8; break;
			case 5: arg = r->r9; break;
			default: arg = 0; break;
		}
		if (arg == 0)
			printf("0");
		else
			printf("0x%lx", arg);
		if (i < nb_args - 1)
			printf(", ");
	}
}

/**
 * main - Traces a command, printing raw hex signatures.
 * @argc: Argument count
 * @argv: Argument vector
 * @envp: Environment variables
 * Return: 0 on success
 */
int main(int argc, char **argv, char **envp)
{
	pid_t pid;
	int status, is_entry = 0, nb_args;
	struct user_regs_struct r;

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
	printf("execve(0, 0, 0) = 0\n");
	while (1)
	{
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		wait(&status);
		if (WIFEXITED(status))
			break;
		if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
		{
			ptrace(PTRACE_GETREGS, pid, NULL, &r);
			nb_args = syscalls_64[r.orig_rax].def; /* Adjust if field has a different name */
			if (!is_entry)
			{
				printf("%s(", syscalls_64[r.orig_rax].name);
				print_args(&r, nb_args);
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
