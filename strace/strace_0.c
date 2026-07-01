#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>

/**
 * main - Executes and traces a given command, printing syscall numbers.
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

	pid = fork();
	if (pid == 0)
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execve(argv[1], argv + 1, envp);
		exit(1);
	}
	else
	{
		wait(&status); /* Wait for initial execve trap */
		ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
		while (1)
		{
			ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
			wait(&status);
			if (WIFEXITED(status))
				break;
			/* TRACESYSGOOD sets bit 7 (0x80) on the signal if it's a syscall trap */
			if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
			{
				ptrace(PTRACE_GETREGS, pid, NULL, &regs);
				if (is_entry)
					printf("%lu\n", (unsigned long)regs.orig_rax);
				is_entry = !is_entry;
			}
		}
	}
	return (0);
}
