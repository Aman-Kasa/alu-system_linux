#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include "syscalls.h"

/**
 * main - Traces a command, printing syscall names and return values.
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
	int is_entry = 0;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (1);
	}
	setbuf(stdout, NULL);
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
		printf("execve = 0\n");

		while (1)
		{
			ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
			wait(&status);
			if (WIFEXITED(status))
			{
				if (is_entry)
					printf("?\n");
				break;
			}
			if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
			{
				ptrace(PTRACE_GETREGS, pid, NULL, &r);
				if (!is_entry)
				{
					printf("%s = ", syscalls_64[r.orig_rax].name);
					is_entry = 1;
				}
				else
				{
					if ((long)r.rax == 0)
						printf("0\n");
					else
						printf("0x%lx\n", (unsigned long)r.rax);
					is_entry = 0;
				}
			}
		}
	}
	return (0);
}
