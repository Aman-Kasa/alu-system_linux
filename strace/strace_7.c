#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include "syscalls.h"

void print_child_string(pid_t pid, unsigned long addr)
{
	char buf[4096];
	size_t i = 0;
	long word;
	while (i < sizeof(buf) - 1)
	{
		word = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
		if (word == -1) break;
		memcpy(buf + i, &word, 8);
		if (memchr(&word, '\0', 8)) break;
		i += 8;
	}
	printf("\"%s\"", buf);
}

void print_child_buffer(pid_t pid, unsigned long addr, unsigned long size)
{
	unsigned char buf[36];
	unsigned long i, limit = (size > 32) ? 32 : size;
	long word;

	for (i = 0; i < limit; i++)
	{
		if (i % 8 == 0)
			word = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
		buf[i] = ((unsigned char *)&word)[i % 8];
	}

	printf("\"");
	for (i = 0; i < limit; i++)
	{
		if (buf[i] == '\n') printf("\\n");
		else if (buf[i] == '\t') printf("\\t");
		else if (buf[i] == '\r') printf("\\r");
		else if (buf[i] >= 32 && buf[i] <= 126) printf("%c", buf[i]);
		else printf("\\%03o", buf[i]);
	}
	printf("\"%s", (size > 32) ? "..." : "");
}

int main(int argc, char **argv, char **envp)
{
	pid_t pid;
	int status, is_entry = 0;
	struct user_regs_struct r;
	const char *nm;

	if (argc < 2) return (1);
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
	int env_c = 0;
	while (envp[env_c]) env_c++;
	printf("execve(\"%s\", [\"%s\"", argv[1], argv[1]);
	if (argv[2]) printf(", \"%s\"", argv[2]);
	printf("], [/* %d vars */]) = 0\n", env_c);

	while (1)
	{
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		wait(&status);
		if (WIFEXITED(status)) break;
		if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
		{
			ptrace(PTRACE_GETREGS, pid, NULL, &r);
			nm = syscalls_64[r.orig_rax].name;
			if (!is_entry)
			{
				if (strcmp(nm, "read") == 0 || strcmp(nm, "write") == 0)
				{
					printf("%s(%ld, ", nm, (long)r.rdi);
					print_child_buffer(pid, r.rsi, r.rdx);
					printf(", %ld) = ", (long)r.rdx);
				}
				else
				{
					printf("%s(", nm);
					if (strcmp(nm, "access") == 0) { print_child_string(pid, r.rdi); printf(r.rsi == F_OK ? ", F_OK" : ", R_OK"); }
					else if (strcmp(nm, "open") == 0) { print_child_string(pid, r.rdi); printf(", O_RDONLY|O_CLOEXEC"); }
					else if (strcmp(nm, "mmap") == 0) { printf(r.rdi == 0 ? "NULL, %ld, " : "0x%lx, %ld, ", (long)r.rdi, (long)r.rsi); int p=0; if(r.rdx&PROT_READ){printf("PROT_READ"); p=1;} if(r.rdx&PROT_WRITE){if(p)printf("|");printf("PROT_WRITE");} printf(", MAP_PRIVATE"); if(r.r10&MAP_ANONYMOUS)printf("|MAP_ANONYMOUS"); printf(", %ld, %ld", (long)r.r8, (long)r.r9); }
					else if (strcmp(nm, "brk") == 0) printf(r.rdi == 0 ? "0" : "0x%lx", (unsigned long)r.rdi);
					else { int n = syscalls_64[r.orig_rax].def, i; unsigned long args[6]; args[0]=r.rdi; args[1]=r.rsi; args[2]=r.rdx; args[3]=r.r10; args[4]=r.r8; args[5]=r.r9; for(i=0; i<n; i++){ printf("%ld", (long)args[i]); if(i<n-1) printf(", "); } }
					printf(") = ");
				}
				is_entry = 1;
			}
			else
			{
				if ((long)r.rax >= -4095 && (long)r.rax < 0) printf("%ld\n", (long)r.rax);
				else if (strcmp(nm, "mmap") == 0 || strcmp(nm, "brk") == 0) printf("0x%lx\n", (unsigned long)r.rax);
				else printf("%ld\n", (long)r.rax);
				is_entry = 0;
			}
		}
	}
	return (0);
}
