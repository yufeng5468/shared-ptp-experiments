#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int
main(int argc, char **argv)
{
	size_t len = 1 << 21;
	char data[] = {0, 1, 2, 3, 4, 5, 6, 7};
	int fd;
	if ((fd = open("/root/2mfile", O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH)) < 0) {
		perror("open");
		abort();
	}
	for (int i = 0; i < len / sizeof(data); i++) {
		ssize_t written = 0;
		while (written < sizeof(data)) {
			ssize_t wrote = write(fd, &data[written],
			    sizeof(data) - written);
			if (wrote < 0) {
				perror("write");
				close(fd);
				abort();
			}
			written += wrote;
		}
	}
	if (close(fd) != 0) {
		perror("close");
		abort();
	}

	return (0);
}
