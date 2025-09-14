#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "complex.h"
#include "fft.h"

static inline double round_n(double n) { return round(n * 100.0) / 100.0; }

Complex *generate_inputs(int len) {
  Complex *res = malloc(len * sizeof(Complex));
  const double pi_over_len = M_PI / len;
  Complex *ptr = res;
  for (int i = 0; i < len; i++, ptr++) {
    double theta = i * pi_over_len;
    double re = cos(10.0 * theta) + 0.5 * cos(25.0 * theta);
    double im = sin(10.0 * theta) + 0.5 * sin(25.0 * theta);
    ptr->real = round_n(re);
    ptr->imag = round_n(im);
  }

  return res;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <size>\n", argv[0]);
    return 1;
  }

  int size = atoi(argv[1]);
  if (size < 0) {
    fprintf(stderr, "invalid <size>; must be a non-negative integer\n");
    return 1;
  }

  int n = 1 << size;

  Complex *signals = generate_inputs(n);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  fft(signals, n);

  clock_gettime(CLOCK_MONOTONIC, &end);

  double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 +
                      (end.tv_nsec - start.tv_nsec) / 1000000.0;

  if (argc > 2) {
  } else {
    printf("execution time: %.3f ms\n", elapsed_ms);
  }

  free(signals);
  return 0;
}
