#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("usage: %s filename\n", argv[0]);
		return (-1);
	}
	printf("filename: %s\n", argv[1]);

	int fd = open(argv[1], O_RDONLY);
	struct stat stat;
	if (fd < 0 || fstat(fd, &stat) < 0) {
                fprintf(stderr, "open() or fstat() failed.\n");
                return (-1);
        }

	char *addr;
	if ((addr = mmap(0, stat.st_size, PROT_READ, MAP_PRIVATE | MAP_ALIGNED_SUPER, fd, 0)) == MAP_FAILED) {
		perror("mmap");
		return (-1);
	}

	int num_pages = stat.st_size / 4096;
	int num_superpages = num_pages / 512;

	char *vec = malloc(num_pages * sizeof(*vec));
	if (mincore(addr, num_pages * 0x1000, vec) != 0) {
		perror("mincore");
	}

	for (int i = 0; i < num_superpages; i++) {
		bool is_superpage = true;
		if ((vec[i * 512] & MINCORE_SUPER) == MINCORE_SUPER) {
			for (int j = 0; j < 512; j++) {
				if ((vec[i * 512 + j] & MINCORE_SUPER) != MINCORE_SUPER) {
					printf("pindex %d has MINCORE_SUPER set, but pindex %d doesn't\n", i * 512, i * 512 + j);
					is_superpage = false;
				}
			}
		} else {
			is_superpage = false;
		}
		if (is_superpage) {
			printf("superpage %d has MINCORE_SUPER fully set\n", i);
		} else {
			printf("superpage %d doesn't have MINCORE_SUPER fully set\n", i);
		}
	}

	return (0);
}
