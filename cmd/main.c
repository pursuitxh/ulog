#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


#define PAGE_SIZE (4*1024)
#define BUF_SIZE (32*PAGE_SIZE)
#define OFFSET (0)

static int bl_log_help(int argc, char *argv[]);
static int bl_log_version(int argc, char *argv[]);
static int bl_log_read(int argc, char *argv[]);

static const char usbip_usage_string[] =
	"bl_log [--debug] [--log] [--set PORT] [version]\n"
	"             [help] <command> <args>\n";

static const char bl_log_version_string[] = "0.0.0.1";

struct command {
	const char *name;
	int (*fn)(int argc, char *argv[]);
	const char *help;
	void (*usage)(void);
};

static const struct command cmds[] = {
	{
		.name  = "read",
		.fn    = bl_log_read,
		.help  = NULL,
		.usage = NULL
	},
	{
		.name  = "help",
		.fn    = bl_log_help,
		.help  = NULL,
		.usage = NULL
	},
	{
		.name  = "version",
		.fn    = bl_log_version,
		.help  = NULL,
		.usage = NULL
	},
	{ NULL, NULL, NULL, NULL }
};

static void bl_log_usage(void)
{
	printf("usage: %s", usbip_usage_string);
}

static int bl_log_read(int argc, char *argv[])
{
	int fd;
	char *addr = NULL;

	fd = open("/dev/bl_log", O_RDWR);
	if (fd < 0) {
		perror("open failed\n");
		exit(-1);
	}

	addr = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, OFFSET);
	if (!addr) {
		perror("mmap failed\n");
		exit(-1);
	}

	printf("%s\n", addr);

	munmap(addr, BUF_SIZE);
	close(fd);

	return 0;
}

static int bl_log_help(int argc, char *argv[])
{
	const struct command *cmd;
	int i;
	int ret = 0;

	if (argc > 1 && argv++) {
		for (i = 0; cmds[i].name != NULL; i++)
			if (!strcmp(cmds[i].name, argv[0]) && cmds[i].usage) {
				cmds[i].usage();
				goto done;
			}
		ret = -1;
	}

	bl_log_usage();
	printf("\n");
	for (cmd = cmds; cmd->name != NULL; cmd++)
		if (cmd->help != NULL)
			printf("  %-10s %s\n", cmd->name, cmd->help);
	printf("\n");
done:
	return ret;
}

static int bl_log_version(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	printf(" (%s)\n", bl_log_version_string);
	return 0;
}



static int run_command(const struct command *cmd, int argc, char *argv[])
{
	printf("running command: `%s'", cmd->name);
	return cmd->fn(argc, argv);
}

int main(int argc, char *argv[])
{
	static const struct option opts[] = {
		{ "tag", 		required_argument, NULL, 't' },
		{ "filter", 	required_argument, NULL, 'f' },
		{ NULL,      	0,                 NULL,  0  }
	};

	char *cmd;
	int opt;
	int i, rc = -1;

	opterr = 0;
	for (;;) {
		opt = getopt_long(argc, argv, "+t:f:", opts, NULL);

		if (opt == -1)
			break;

		switch (opt) {
		case 't':
			break;
		case 'f':
			break;
		case '?':
			printf("invalid option\n");
		default:
			bl_log_usage();
			goto out;
		}
	}

	cmd = argv[optind];
	if (cmd) {
		for (i = 0; cmds[i].name != NULL; i++)
			if (!strcmp(cmds[i].name, cmd)) {
				argc -= optind;
				argv += optind;
				optind = 0;
				rc = run_command(&cmds[i], argc, argv);
				goto out;
			}
	}

	/* invalid command */
	bl_log_help(0, NULL);
out:
	return (rc > -1 ? EXIT_SUCCESS : EXIT_FAILURE);
}


