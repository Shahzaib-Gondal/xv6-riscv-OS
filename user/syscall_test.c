#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int
main(void)
{
  uint64 c1 = getcycles();
  uint64 t1 = gettime();
  uint64 i1 = getinstret();

  for (volatile int i = 0; i < 100000; i++);

  uint64 c2 = getcycles();
  uint64 t2 = gettime();
  uint64 i2 = getinstret();

  printf("Cycles: %ld\n", c2 - c1);
  printf("Time: %ld\n", t2 - t1);
  printf("Instructions Retired: %ld\n", i2 - i1);
  exit(0);
}
