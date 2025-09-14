#ifndef COMPLEX_HPP
#define COMPLEX_HPP

#include <cmath>

struct Complex {
  double real;
  double imag;

  Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) {}

  Complex operator+(const Complex &other) const {
    return Complex(real + other.real, imag + other.imag);
  }

  Complex operator-(const Complex &other) const {
    return Complex(real - other.real, imag - other.imag);
  }

  Complex operator*(const Complex &other) const {
    return Complex(real * other.real - imag * other.imag,
                   real * other.imag + imag * other.real);
  }

  Complex operator*(double scalar) const {
    return Complex(real * scalar, imag * scalar);
  }
};

#endif // COMPLEX_HPP
