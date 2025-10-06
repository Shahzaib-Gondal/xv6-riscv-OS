#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// multiply C = A * B.
static void
matmul(int *A, int *B, int *C, int N) {
  int i, j, k;
  for (i = 0; i < N; i++) {
    int basei = i * N;
    for (j = 0; j < N; j++) {
      long sum = 0;
      for (k = 0; k < N; k++) {
        sum += (long)A[basei + k] * (long)B[k * N + j];
      }
      C[basei + j] = (int)sum;
    }
  }
}

int
main(int argc, char **argv) {
  // if no arguments, we test with this
  int sizes[] = {10, 50, 250, 500}; // N x N
  int nsizes = sizeof(sizes) / sizeof(sizes[0]);

  // if user supplied sizes as args, we test with the arguments at hand
  if (argc > 1) {
    nsizes = 0;
    for (int i = 1; i < argc && nsizes < 16; i++) {
      sizes[nsizes++] = atoi(argv[i]);
    }
  }
  // this for loop goes over all testing sizes
  for (int si = 0; si < nsizes; si++) {
    int N = sizes[si];
    uint64 total_elems = (uint64)N * (uint64)N; // type conversion
    uint64 bytes_needed = total_elems * sizeof(int) * 3; // 3 matrices

    printf("== matbench: N=%d  (need ~%d KB) ==\n", N, (int)(bytes_needed/1024));

    // malloc used
    int *A = malloc(total_elems * sizeof(int));
    int *B = malloc(total_elems * sizeof(int));
    int *C = malloc(total_elems * sizeof(int));
    // we can check at this stage if we ran out of memory
    if (!A || !B || !C) {
      printf("allocation failed at N=%d (malloc returned 0). Try smaller size.\n", N);
      if (A) free(A);
      if (B) free(B);
      if (C) free(C);
      continue;
    }

    // initialize (and touch) so pages are mapped
    for (uint64 i = 0; i < total_elems; i++) {
      A[i] = 1;            // simple values to keep results small
      B[i] = (i % 8) + 1;  // some variety so we don't only multiply same vals
      C[i] = 0;
    }

    // a test run to intialize everything in place.
    matmul(A, B, C, N);

    // measure
    uint64 s_cycle = getcycles();
    uint64 s_time = gettime();
    uint64 s_inst  = getinstret();

    matmul(A, B, C, N);

    uint64 e_cycle = getcycles();
    uint64 e_time  = gettime();
    uint64 e_inst   = getinstret();

    uint64 d_cycle = e_cycle - s_cycle;
    uint64 d_time  = e_time  - s_time;
    uint64 d_inst  = e_inst  - s_inst;

    // print
    printf("N=%d, cycles=%ld, time_ticks=%ld, instret=%ld\n",
           N, d_cycle, d_time, d_inst);

    // free
    free(A); free(B); free(C);
  }

  exit(0);
}