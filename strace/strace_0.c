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

	/* * Disable buffering on stdout to ensure tracer output interleaves
	 * correctly with the tracee's output (fixes the output race condition) 
	 */
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
		/* Catch the initial stop caused by execve */
		wait(&status);
		ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);

		/* * The first stop IS the execve syscall (59). We must retrieve
		 * the registers and print it here before entering our loop.
		 */
		ptrace(PTRACE_GETREGS, pid, NULL, &regs);
		printf("%lu\n", (unsigned long)regs.orig_rax);

		while (1)
		{
			ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
			wait(&status);

			if (WIFEXITED(status))
				break;

			/* Check if stopped by a syscall trap (bit 7 set) */
			if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
			{
				if (is_entry)
				{
					ptrace(PTRACE_GETREGS, pid, NULL, &regs);
					printf("%lu\n", (unsigned long)regs.orig_rax);
				}
				/* Toggle state to ignore the syscall exit */
				is_entry = !is_entry;
			}
		}
	}
	return (0);
}
