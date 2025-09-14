#ifndef COMPLEX_H
#define COMPLEX_H

typedef struct {
  double real;
  double imag;
} Complex;

// Inline functions for better performance
static inline Complex complex_add(Complex a, Complex b) {
  Complex result = {a.real + b.real, a.imag + b.imag};
  return result;
}

static inline Complex complex_sub(Complex a, Complex b) {
  Complex result = {a.real - b.real, a.imag - b.imag};
  return result;
}

static inline Complex complex_mul(Complex a, Complex b) {
  Complex result = {a.real * b.real - a.imag * b.imag,
                    a.real * b.imag + a.imag * b.real};
  return result;
}

static inline Complex complex_mul_scalar(Complex a, double scalar) {
  Complex result = {a.real * scalar, a.imag * scalar};
  return result;
}

static inline Complex complex_init(double real, double imag) {
  Complex result = {real, imag};
  return result;
}

#endif // COMPLEX_H
