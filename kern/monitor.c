// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
	{ "backtrace", "Display information about the backtrace", mon_backtrace },
	{ "mem", "Dispaly information about the mem k-kernl u-user s-stab", mon_kernelMemLayout },
};
#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < NCOMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.
	
	uintptr_t eip = tf->tf_eip;
	struct Eipdebuginfo info;
    debuginfo_eip(eip, &info);
    
	return 0;
}

int
mon_kernelMemLayout(int argc, char **argv, struct Trapframe *tf)
{
	int mode = 1;

	if(argc > 1)
	{
		cprintf(argv[1]);
		if(argv[1][0] == 'k')
		{
			mode |= 1;
		}else if(argv[1][0] == 'u')
		{
			mode |= 2;
		}else if(argv[1][0] == 's')
		{
			mode |= 4;
		}
	}

	if((mode & 1) != 0)
	{
			cprintf(
			"*\n"
			"* Virtual memory map:                                Permissions\n"
			"*                                                    kernel/user\n"
			"*\n"
			"*    4 Gig -------->  +------------------------------+\n"
			"*                     |                              | RW/--\n"
			"*                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
			"*                     :              .               :\n"
			"*                     :              .               :\n"
			"*                     :              .               :\n"
			"*                     |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| RW/--\n"
			"*                     |                              | RW/--\n"
			"*                     |   Remapped Physical Memory   | RW/--\n"
			"*                     |                              | RW/--\n"
			"*    KERNBASE, ---->  +------------------------------+ 0xf0000000      --+\n"
			"*    KSTACKTOP        |     CPU0's Kernel Stack      | RW/--  KSTKSIZE   |\n"
			"*                     | - - - - - - - - - - - - - - -|                   |\n"
			"*                     |      Invalid Memory (*)      | --/--  KSTKGAP    |\n"
			"*                     +------------------------------+                   |\n"
			"*                     |     CPU1's Kernel Stack      | RW/--  KSTKSIZE   |\n"
			"*                     | - - - - - - - - - - - - - - -|                 PTSIZE\n"
			"*                     |      Invalid Memory (*)      | --/--  KSTKGAP    |\n"
			"*                     +------------------------------+                   |\n"
			"*                     :              .               :                   |\n"
			"*                     :              .               :                   |\n"
			"*    MMIOLIM ------>  +------------------------------+ 0xefc00000      --+\n"
			"*                     |       Memory-mapped I/O      | RW/--  PTSIZE\n"
			"* ULIM, MMIOBASE -->  +------------------------------+ 0xef800000\n"
			);
	}
	if((mode & 2) != 0)
	{
			cprintf(
			"*   MMIOLIM ------>  +------------------------------+ 0xefc00000      --+\n"
			"*                     |       Memory-mapped I/O      | RW/--  PTSIZE\n"
			"* ULIM, MMIOBASE -->  +------------------------------+ 0xef800000\n"
			"*                     |  Cur. Page Table (User R-)   | R-/R-  PTSIZE\n"
			"*    UVPT      ---->  +------------------------------+ 0xef400000\n"
			"*                     |          RO PAGES            | R-/R-  PTSIZE\n"
			"*    UPAGES    ---->  +------------------------------+ 0xef000000\n"
			"*                     |           RO ENVS            | R-/R-  PTSIZE\n"
			"* UTOP,UENVS ------>  +------------------------------+ 0xeec00000\n"
			"* UXSTACKTOP -/       |     User Exception Stack     | RW/RW  PGSIZE\n"
			"*                     +------------------------------+ 0xeebff000\n"
			"*                     |       Empty Memory (*)       | --/--  PGSIZE\n"
			"*    USTACKTOP  --->  +------------------------------+ 0xeebfe000\n"
			"*                     |      Normal User Stack       | RW/RW  PGSIZE\n"
			"*                     +------------------------------+ 0xeebfd000\n"
			"*                     |                              |\n"
			"*                     |                              |\n"
			"*                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
			"*                     .                              .\n"
			"*                     .                              .\n"
			);
	}
	if((mode & 4) != 0)
	{
			cprintf(
			"*                     .                              .\n"
			"*                     .                              .\n"
			"*                     |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\n"
			"*                     |     Program Data & Heap      |\n"
			"*    UTEXT -------->  +------------------------------+ 0x00800000\n"
			"*    PFTEMP ------->  |       Empty Memory (*)       |        PTSIZE\n"
			"*                     |                              |\n"
			"*    UTEMP -------->  +------------------------------+ 0x00400000      --+\n"
			"*                     |       Empty Memory (*)       |                   |\n"
			"*                     | - - - - - - - - - - - - - - -|                   |\n"
			"*                     |  User STAB Data (optional)   |                 PTSIZE\n"
			"*    USTABDATA ---->  +------------------------------+ 0x00200000        |\n"
			"*                     |       Empty Memory (*)       |                   |\n"
			"*    0 ------------>  +------------------------------+                 --+\n"
			);
	}

	return 0;
}


/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
