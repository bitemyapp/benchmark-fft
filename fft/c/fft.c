#include "fft.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void fft_recursive(Complex *arr, int n) {
  if (n == 1) {
    return;
  }

  Complex *a0 = malloc((n / 2) * sizeof(Complex));
  Complex *a1 = malloc((n / 2) * sizeof(Complex));

  Complex *src = arr;
  Complex *even = a0;
  Complex *odd = a1;

  int half_n = n / 2;
  for (int i = 0; i < half_n; i++) {
    even[i] = src[2 * i];
    odd[i] = src[2 * i + 1];
  }

  fft_recursive(a0, half_n);
  fft_recursive(a1, half_n);

  double ang = -2.0 * M_PI / n;
  Complex w = complex_init(1.0, 0.0);
  Complex wn = complex_init(cos(ang), sin(ang));

  for (int i = 0; i < half_n; i++) {
    Complex p = a0[i];
    Complex q = complex_mul(w, a1[i]);
    arr[i] = complex_add(p, q);
    arr[i + half_n] = complex_sub(p, q);
    w = complex_mul(w, wn);
  }

  free(a0);
  free(a1);
}

void fft(Complex *arr, int n) {
  fft_recursive(arr, n);

  double factor = 1.0 / sqrt(n);

  for (int i = 0; i < n; i++) {
    arr[i].real *= factor;
    arr[i].imag *= factor;
  }
}
