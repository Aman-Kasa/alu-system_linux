#include "syscalls.h"

static const flag_t open_acc_g[] = {
	{"O_RDONLY", 0}, {"O_WRONLY", 1}, {"O_RDWR", 2}
};
static const flag_t open_flags_g[] = {
	{"O_CREAT", 0100}, {"O_EXCL", 0200}, {"O_NOCTTY", 0400},
	{"O_TRUNC", 01000}, {"O_APPEND", 02000}, {"O_NONBLOCK", 04000},
	{"O_DSYNC", 010000}, {"O_DIRECT", 040000}, {"O_LARGEFILE", 0100000},
	{"O_DIRECTORY", 0200000}, {"O_NOFOLLOW", 0400000},
	{"O_NOATIME", 01000000}, {"O_CLOEXEC", 02000000}, {"O_PATH", 010000000}
};
static const flag_t access_flags_g[] = {
	{"R_OK", 4}, {"W_OK", 2}, {"X_OK", 1}
};
static const flag_t prot_flags_g[] = {
	{"PROT_READ", 0x1}, {"PROT_WRITE", 0x2}, {"PROT_EXEC", 0x4}
};
static const flag_t map_base_g[] = {
	{"MAP_SHARED", 0x01}, {"MAP_PRIVATE", 0x02}
};
static const flag_t map_flags_g[] = {
	{"MAP_FIXED", 0x10}, {"MAP_ANONYMOUS", 0x20},
	{"MAP_GROWSDOWN", 0x0100}, {"MAP_DENYWRITE", 0x0800},
	{"MAP_EXECUTABLE", 0x1000}, {"MAP_LOCKED", 0x2000},
	{"MAP_NORESERVE", 0x4000}, {"MAP_POPULATE", 0x8000},
	{"MAP_NONBLOCK", 0x10000}, {"MAP_STACK", 0x20000},
	{"MAP_HUGETLB", 0x40000}
};

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
 * print_buffer - prints a read/write buffer, escaping non-printables and
 *                truncating after 32 characters
 * @pid: pid of the traced process
 * @addr: remote address of the buffer
 * @len: number of bytes requested for the syscall
 **/
void print_buffer(pid_t pid, unsigned long addr, size_t len)
{
	static const char *esc[] = {"\\a", "\\b", "\\t", "\\n", "\\v", "\\f",
		"\\r"};
	size_t i, n, w;
	long word;
	unsigned char byte;

	if (addr == 0)
	{
		printf("NULL");
		return;
	}
	n = len < 32 ? len : 32;
	putchar('"');
	for (i = 0; i < n; i++)
	{
		if (i % sizeof(long) == 0)
			word = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
		byte = ((unsigned char *)&word)[i % sizeof(long)];
		if (byte >= '\a' && byte <= '\r' && byte != '\v' - 1)
			printf("%s", esc[byte - '\a']);
		else if (byte == '\\' || byte == '"')
			printf("\\%c", byte);
		else if (byte >= 32 && byte < 127)
			putchar(byte);
		else
			printf("\\%03o", byte);
	}
	putchar('"');
	if (len > 32)
		printf("...");
	(void)w;
}

/**
 * print_flags - prints an OR-combination of flag macros for a value
 * @val: the raw value
 * @base: table of mutually exclusive base values (may be NULL)
 * @basen: size of base table
 * @flags: table of OR-able bit flags (may be NULL)
 * @flagsn: size of flags table
 * @zero: name to print when val is 0 (may be NULL)
 **/
void print_flags(unsigned long val, const flag_t *base, size_t basen,
	const flag_t *flags, size_t flagsn, const char *zero)
{
	size_t i;
	int printed = 0;

	if (val == 0 && zero != NULL)
	{
		printf("%s", zero);
		return;
	}
	for (i = 0; i < basen; i++)
		if ((val & 3) == (unsigned long)base[i].value)
		{
			printf("%s", base[i].name);
			printed = 1;
			break;
		}
	for (i = 0; i < flagsn; i++)
		if (val & (unsigned long)flags[i].value)
		{
			printf("%s%s", printed ? "|" : "", flags[i].name);
			printed = 1;
		}
	if (!printed)
		printf("%#lx", val);
}

/**
 * print_one_arg - prints a single value (parameter or return) by type,
 *                 interpreting NULL, buffers, and known flag macros
 * @name: syscall name, used to special-case flags/buffers (may be "")
 * @idx: parameter index, or (size_t)-1 for a return value
 * @type: the type to use for formatting
 * @val: the raw register value
 * @len: byte-length hint for buffer params (read/write), else 0
 * @pid: pid of the traced process
 **/
void print_one_arg(const char *name, size_t idx, type_t type, size_t val,
	size_t len, pid_t pid)
{
	char *str;

	if ((!strcmp(name, "read") || !strcmp(name, "write")) && idx == 1)
		print_buffer(pid, val, len);
	else if (type == CHAR_P)
	{
		str = (val == 0) ? NULL : read_string(pid, val);
		printf(str == NULL ? "NULL" : "\"%s\"", str);
	}
	else if (IS_POINTER(type))
		printf(val == 0 ? "NULL" : "%#lx", val);
	else if (!strcmp(name, "open") ? idx == 1 :
		!strcmp(name, "openat") && idx == 2)
		print_flags(val, open_acc_g, 3, open_flags_g, 14, NULL);
	else if ((!strcmp(name, "access") && idx == 1) ||
		(!strcmp(name, "faccessat") && idx == 2))
		print_flags(val, NULL, 0, access_flags_g, 3, "F_OK");
	else if (!strcmp(name, "mmap") && idx == 2)
		print_flags(val, NULL, 0, prot_flags_g, 3, "PROT_NONE");
	else if (!strcmp(name, "mmap") && idx == 3)
		print_flags(val, map_base_g, 2, map_flags_g, 11, NULL);
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
 * main - traces a process and prints its system calls
 * @argc: argument count
 * @argv: argument array
 * @envp: environment parameters
 * Return: 0 on success | 1 on failure (not enough arguments)
 **/
int main(int argc, char *argv[], char *envp[])
{
	int skip, status;
	size_t i, envc, params[MAX_PARAMS];
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
			sc = get_syscall(regs.orig_rax);
			if (skip)
			{
				if (sc == NULL)
				{
					printf("\nsyscall_%llu(", regs.orig_rax);
					continue;
				}
				printf("\n%s(", sc->name);
				params[0] = regs.rdi, params[1] = regs.rsi;
				params[2] = regs.rdx, params[3] = regs.r10;
				params[4] = regs.r8, params[5] = regs.r9;
				for (i = 0; sc->params[0] != VOID && i < sc->nb_params; i++)
				{
					if (i)
						printf(", ");
					if (sc->params[i] == VARARGS)
						printf("...");
					else
						print_one_arg(sc->name, i, sc->params[i],
							params[i], params[2], pid);
				}
			}
			else if (WIFEXITED(status))
				printf(") = ?\n");
			else if (sc == NULL || IS_POINTER(sc->ret))
				printf(") = %#llx", regs.rax);
			else
			{
				printf(") = ");
				print_one_arg("", (size_t)-1, sc->ret, regs.rax, 0, pid);
			}
		}
	}

	return (0);
}
