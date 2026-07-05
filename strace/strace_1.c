#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include "syscalls.h"

/**
 * main - Executes and traces a given command, printing syscall names.
 * @argc: Argument count
 * @argv: Argument vector
 * @envp: Environment variables
 * Return: 0 on success, 1 on failure
 */
int main(int argc, char **argv, char **envp)
{
	pid_t pid;
	int status;
	struct user_regs_struct regs;
	int is_entry = 1;

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

		ptrace(PTRACE_GETREGS, pid, NULL, &regs);
		printf("%s\n", syscalls_64[regs.orig_rax].name);

		while (1)
		{
			ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
			wait(&status);

			if (WIFEXITED(status))
				break;

			if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
			{
				if (is_entry)
				{
					ptrace(PTRACE_GETREGS, pid, NULL, &regs);
					printf("%s\n", syscalls_64[regs.orig_rax].name);
				}
				is_entry = !is_entry;
			}
		}
	}
	return (0);
}
