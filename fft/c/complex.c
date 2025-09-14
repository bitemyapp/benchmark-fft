#include "complex.h"

Complex complex_add(Complex a, Complex b) {
  Complex result;
  result.real = a.real + b.real;
  result.imag = a.imag + b.imag;
  return result;
}

Complex complex_sub(Complex a, Complex b) {
  Complex result;
  result.real = a.real - b.real;
  result.imag = a.imag - b.imag;
  return result;
}

Complex complex_mul(Complex a, Complex b) {
  Complex result;
  result.real = a.real * b.real - a.imag * b.imag;
  result.imag = a.real * b.imag + a.imag * b.real;
  return result;
}

Complex complex_mul_scalar(Complex a, double scalar) {
  Complex result;
  result.real = a.real * scalar;
  result.imag = a.imag * scalar;
  return result;
}
