#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

int
main(int argc, char **argv)
{
	char *start;
	size_t len = (1 << 21) - 4096;
	int fd;

	if ((fd = open("/root/2mfile", O_RDONLY)) < 0) {
		perror("open");
		abort();
	}

	if ((start = mmap(0, len, PROT_READ, MAP_SHARED | 0x2000, fd, 0)) ==
	    MAP_FAILED) {
		perror("mmap");
		abort();
	}

	// Read something in the parent process.
	for (int i = 0; i < len; ++i) {
		if (start[i] != (i % 8)) {
			fprintf(stderr, "inconsistent data");
			return (-1);
		}
	}

	return (0);
}
