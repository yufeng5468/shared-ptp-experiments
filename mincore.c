/*
 * Usage: ./a.out filepath
 *
 * Given a file, thie program prints out the file's memory-residency status at
 * a base page granularity in the form of a bitmap.  The program also reports
 * superpage mappings of various supported sizes.
 *
 * Created: 2020-02-26
 * Last Modified: 2021-07-06
 * Alan L. Cox <alc@rice.edu>
 * Yufeng Zhou <yufengz@rice.edu>
 */

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BITS_PER_ROW 16
#define NUM_4KPTE_PER_2MPDE 512

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s filepath\n", argv[0]);
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

	/* Get page sizes. */
	int num_page_sizes;
	if ((num_page_sizes = getpagesizes(NULL, 0)) < 0) {
		perror("getpagesizes(NULL, 0)");
		return (-1);
	}
	if (num_page_sizes == 0) {
		fprintf(stderr, "num_page_sizes == 0\n");
		return (-1);
	}
	size_t *page_sizes = malloc(num_page_sizes * sizeof(*page_sizes));
	if (getpagesizes(page_sizes, num_page_sizes) != num_page_sizes) {
		perror("getpagesizes");
		return (-1);
	}

	int num_pages = stat.st_size / page_sizes[0];
	num_pages += (stat.st_size % page_sizes[0]) ? 1 : 0;

	char *vec = malloc(num_pages * sizeof(*vec));
	if (mincore(addr, num_pages * page_sizes[0], vec) != 0) {
		perror("mincore");
	}

	/* Report 4K page residency. */
	for (int i = 0; i < num_pages; i++) {
		if ((i % NUM_4KPTE_PER_2MPDE) == 0) {
			printf("\n");
		}
		if ((i % BITS_PER_ROW) == 0) {
			printf("pindex [%6d - %6d]: ", i, i + BITS_PER_ROW - 1);
		}

		if (vec[i] & MINCORE_INCORE) {
			printf("1");
		} else {
			printf("0");
		}

		if (((i + 1) % BITS_PER_ROW) == 0) {
			printf("\n");
		}
	}
	printf("\n\n");

	if (num_page_sizes < 2) {
		printf("Superpages are not supported or enabled on this system\n");
		return (0);
	}

	/* Report superpage mappings. */
	/*
	 * MINCORE_PSIND() introduced in FreeBSD 13.0.
	 * Prior to that, MINCORE_SUPER was a single-bit mask.
	 */
	int num_superpages = 0;
	for (int i = 0; i < num_pages; i++) {
		if (vec[i] & MINCORE_SUPER) {
			int j = 1;
			while (!(vec[i] & MINCORE_PSIND(j))) {
				j++;
			}
			size_t superpage_size = page_sizes[j];
			int num_base_pages = superpage_size / page_sizes[0];
			bool is_superpage = true;
			for (int k = 0; k < num_base_pages; k++) {
				if (!(vec[i + k] & MINCORE_PSIND(j))) {
					fprintf(stderr, "pindex %d has MINCORE_PSIND(%d) set, but pindex %d doesn't\n", i, j, i + k);
					is_superpage = false;
				}
			}
			if (is_superpage) {
				printf("pindex [%6d - %6d] is mapped by psind %d of size %zu bytes\n", i, i + num_base_pages - 1, j, superpage_size);
				num_superpages++;
				i += num_base_pages - 1;
			}
		}
	}

	if (num_superpages) {
		printf("Found a total of %d superpage mappings\n", num_superpages);
	} else {
		if (stat.st_size / page_sizes[1] > 0) {
			printf("Superpage-sized regions exist but no superpage mappings found\n");
		} else {
			printf("Superpage-sized regions do NOT exist\n");
		}
	}

	return (0);
}
