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
 * @sc: Pointer to the specific syscall table entry
 */
void print_args(struct user_regs_struct *r, syscall_t *sc)
{
	int i;
	unsigned long arg;

	for (i = 0; i < sc->def; i++) /* Use your struct's argument count field */
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
		
		if (i < sc->def - 1)
			printf(", ");
	}
	/* Note: Depending on your provided header, you may need a condition here 
	   to check if the syscall is variadic and print ", ..." */
}

/**
 * main - Traces a command, printing syscall names, args, and returns.
 * @argc: Argument count
 * @argv: Argument vector
 * @envp: Environment variables
 * Return: 0 on success, 1 on failure
 */
int main(int argc, char **argv, char **envp)
{
	pid_t pid;
	int status;
	struct user_regs_struct r;
	int is_entry = 1;
	syscall_t *sc;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (1);
	}

	pid = fork();
	if (pid == 0)
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execve(argv[1], argv + 1, envp);
		exit(1);
	}
	else
	{
		wait(&status);
		ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
		while (1)
		{
			ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
			wait(&status);
			if (WIFEXITED(status))
			{
				if (!is_entry)
					printf("?\n");
				break;
			}
			if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
			{
				ptrace(PTRACE_GETREGS, pid, NULL, &r);
				sc = &syscalls_64[r.orig_rax];
				if (is_entry)
				{
					if (sc->name)
					{
						printf("%s(", sc->name);
						print_args(&r, sc);
						printf(") = ");
					}
					fflush(stdout);
				}
				else
				{
					if ((long)r.rax == 0)
						printf("0\n");
					else
						printf("0x%lx\n", (unsigned long)r.rax);
				}
				is_entry = !is_entry;
			}
		}
	}
	return (0);
}
