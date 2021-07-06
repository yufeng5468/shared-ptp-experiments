#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BITS_PER_ROW 16
#define NUM_PTE_PER_PDE 512

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s filename\n", argv[0]);
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
	if ((addr = mmap(0, stat.st_size, PROT_READ, MAP_PRIVATE | MAP_ALIGNED_SUPER | MAP_PREFAULT_READ, fd, 0)) == MAP_FAILED) {
		perror("mmap");
		return (-1);
	}

	int num_pages = stat.st_size / 4096;
	int num_superpages = num_pages / NUM_PTE_PER_PDE;

	char *vec = malloc(num_pages * sizeof(*vec));
	if (mincore(addr, num_pages * 0x1000, vec) != 0) {
		perror("mincore");
	}

	/* Report 4K page residency. */
	for (int i = 0; i < num_pages; i++) {
		if ((i % NUM_PTE_PER_PDE) == 0) {
			printf("\n");
		}
		if ((i % BITS_PER_ROW) == 0) {
			printf("pindex [");
			printf("%6d", i);
			printf(" - ");
			printf("%6d", i + 15);
			printf("]: ");
		}

		if ((vec[i] & MINCORE_INCORE) == MINCORE_INCORE) {
			printf("1");
		} else {
			printf("0");
		}

		if (((i + 1) % BITS_PER_ROW) == 0) {
			printf("\n");
		}
	}
	printf("\n");

	/* Report superpage mappings. */
	/*
	 * ALC:
	 * Also, at some point, but I forget exactly which version, we changed
	MINCORE_SUPER such that it is no longer a single-bit flag.  It is a bit
	field that encodes the 'psind' value, i.e., the page size index.  So,
	you need to write the MINCORE_SUPER tests as != 0 and == 0.
	 */
	for (int i = 0; i < num_superpages; i++) {
		bool is_superpage = true;
		if ((vec[i * NUM_PTE_PER_PDE] & MINCORE_SUPER) == MINCORE_SUPER) {
			for (int j = 0; j < NUM_PTE_PER_PDE; j++) {
				if ((vec[i * NUM_PTE_PER_PDE + j] & MINCORE_SUPER) != MINCORE_SUPER) {
					printf("pindex %d has MINCORE_SUPER set, but pindex %d doesn't\n", i * NUM_PTE_PER_PDE, i * NUM_PTE_PER_PDE + j);
					is_superpage = false;
				}
			}
		} else {
			is_superpage = false;
		}
		if (is_superpage) {
			printf("superpage %d has MINCORE_SUPER fully set\n", i);
		} else {
			printf("superpage %d does NOT have MINCORE_SUPER fully set\n", i);
		}
	}
	if (num_superpages == 0) {
		printf("No superpage-sized regions\n");
	}

	return (0);
}
