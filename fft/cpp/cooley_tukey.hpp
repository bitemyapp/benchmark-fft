#ifndef COOLEY_TUKEY_HPP
#define COOLEY_TUKEY_HPP

#include "complex.hpp"
#include <cmath>
#include <vector>

void fft(std::vector<Complex> &arr) {
  int n = arr.size();
  if (n == 1)
    return;

  std::vector<Complex> a0(n / 2);
  std::vector<Complex> a1(n / 2);

  for (int i = 0; i < n / 2; i++) {
    a0[i] = arr[2 * i];
    a1[i] = arr[2 * i + 1];
  }

  fft(a0);
  fft(a1);

  double ang = -2.0 * M_PI / n;
  Complex w(1.0, 0.0);
  Complex wn(cos(ang), sin(ang));

  for (int i = 0; i < n / 2; i++) {
    Complex p = a0[i];
    Complex q = w * a1[i];
    arr[i] = p + q;
    arr[i + n / 2] = p - q;
    w = w * wn;
  }
}

#endif // COOLEY_TUKEY_HPP
