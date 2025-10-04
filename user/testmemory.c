#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define CHUNK (1024 * 1024)  // 1 MB per allocation

int
main(void)
{
  printf("Starting memory allocation test...\n");

  uint64 total = 0;
  char *p;

  while (1) {
    p = malloc(CHUNK);
    if (p == 0) {
      printf("\nAllocation failed after allocating %lu MB\n", total / (1024 * 1024));
      break;
    }

    // Touch one byte per page so the memory actually gets mapped
    for (int i = 0; i < CHUNK; i += 4096)
      p[i] = 0;

    total += CHUNK;
  }

  printf("Total successfully allocated memory: %lu MB\n", total / (1024 * 1024));
  exit(0);
}
