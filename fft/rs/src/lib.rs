use rayon::prelude::*;
use std::f64::consts::PI;

// cleaned: only keep primary fft implementation

#[derive(Debug, Clone, Copy, PartialEq)]
pub struct Complex {
    pub real: f64,
    pub imag: f64,
}

impl Complex {
    #[inline]
    pub fn new(real: f64, imag: f64) -> Self {
        Self { real, imag }
    }
}

impl std::ops::Add for Complex {
    type Output = Self;

    #[inline]
    fn add(self, other: Self) -> Self {
        Self {
            real: self.real + other.real,
            imag: self.imag + other.imag,
        }
    }
}

impl std::ops::Sub for Complex {
    type Output = Self;

    #[inline]
    fn sub(self, other: Self) -> Self {
        Self {
            real: self.real - other.real,
            imag: self.imag - other.imag,
        }
    }
}

impl std::ops::Mul for Complex {
    type Output = Self;

    #[inline]
    fn mul(self, other: Self) -> Self {
        Self {
            real: self.real * other.real - self.imag * other.imag,
            imag: self.real * other.imag + self.imag * other.real,
        }
    }
}

impl std::ops::Mul<f64> for Complex {
    type Output = Self;

    #[inline]
    fn mul(self, scalar: f64) -> Self {
        Self {
            real: self.real * scalar,
            imag: self.imag * scalar,
        }
    }
}

fn bit_reverse_permute(arr: &mut [Complex]) {
    let n = arr.len();
    if n <= 1 {
        return;
    }
    debug_assert!(n.is_power_of_two());

    let bits: u32 = n.trailing_zeros();
    for i in 0..n {
        let j = i.reverse_bits() >> (usize::BITS - bits);
        if j > i {
            arr.swap(i, j as usize);
        }
    }
}

pub fn fft(arr: &mut [Complex]) {
    let n = arr.len();
    if n == 0 {
        return;
    }
    debug_assert!(n.is_power_of_two());

    // Bit-reversal permutation
    bit_reverse_permute(arr);

    // Iterative Cooley–Tukey, radix-2 DIT
    let mut len = 2usize;
    while len <= n {
        let ang = -2.0 * PI / (len as f64);
        let (s, c) = ang.sin_cos();
        let wlen = Complex::new(c, s);
        let half = len / 2;

        let mut i = 0usize;
        while i < n {
            let mut w = Complex::new(1.0, 0.0);
            let (_, arr) = arr.split_at_mut(i);
            let (left, right) = arr.split_at_mut(half);
            let mut j = 0usize;
            while j < half {
                let u_mut = &mut left[j];
                let v_mut = &mut right[j];
                let u = *u_mut;
                let v = w * *v_mut;
                *u_mut = u + v;
                *v_mut = u - v;
                w = w * wlen;
                j += 1;
            }
            i += len;
        }

        len <<= 1;
    }

    // Unitary scaling to match previous behavior
    let factor = 1.0 / (n as f64).sqrt();
    for it in arr.iter_mut() {
        *it = *it * factor;
    }
}

/// Parallel variant that processes blocks of size `len` in parallel per stage.
/// Algorithm and scaling match `fft`.
pub fn fft_rayon(arr: &mut [Complex]) {
    let n = arr.len();
    if n == 0 {
        return;
    }
    debug_assert!(n.is_power_of_two());

    bit_reverse_permute(arr);

    let mut len = 2usize;
    while len <= n {
        let ang = -2.0 * PI / (len as f64);
        let (s, c) = ang.sin_cos();
        let wlen = Complex::new(c, s);
        let half = len / 2;

        // Split into blocks of size `len` and process in parallel
        arr.par_chunks_mut(len).for_each(|block| {
            if block.len() < len {
                return;
            }
            let (left, right) = block.split_at_mut(half);
            let mut w = Complex::new(1.0, 0.0);
            let mut j = 0usize;
            while j < half {
                let u_mut = &mut left[j];
                let v_mut = &mut right[j];
                let u = *u_mut;
                let v = w * *v_mut;
                *u_mut = u + v;
                *v_mut = u - v;
                w = w * wlen;
                j += 1;
            }
        });

        len <<= 1;
    }

    let factor = 1.0 / (n as f64).sqrt();
    for it in arr.iter_mut() {
        *it = *it * factor;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use num_complex::Complex64 as NumComplex;
    use rustfft::FftPlanner;

    fn to_num_complex(v: &[Complex]) -> Vec<NumComplex> {
        v.iter()
            .map(|c| NumComplex {
                re: c.real,
                im: c.imag,
            })
            .collect()
    }

    fn from_num_complex(v: &[NumComplex]) -> Vec<Complex> {
        v.iter().map(|c| Complex::new(c.re, c.im)).collect()
    }

    fn approx_eq(a: &[Complex], b: &[Complex], tol: f64) -> bool {
        if a.len() != b.len() {
            return false;
        }
        for (x, y) in a.iter().zip(b.iter()) {
            let dr = (x.real - y.real).abs();
            let di = (x.imag - y.imag).abs();
            if dr > tol || di > tol {
                return false;
            }
        }
        true
    }

    fn gen_signal(n: usize) -> Vec<Complex> {
        let mut v = Vec::with_capacity(n);
        for i in 0..n {
            let theta = i as f64 / n as f64 * PI;
            let re = (10.0 * theta).cos() + 0.5 * (25.0 * theta).cos();
            let im = (10.0 * theta).sin() + 0.5 * (25.0 * theta).sin();
            v.push(Complex::new(re, im));
        }
        v
    }

    #[test]
    fn compare_against_rustfft_multiple_sizes() {
        let sizes = [2usize, 4, 8, 16, 32, 64, 128, 256, 512, 1024];
        for &n in &sizes {
            let mut ours = gen_signal(n);
            let mut theirs = to_num_complex(&ours);

            // Our FFT applies unitary scaling 1/sqrt(n). RustFFT does not by default.
            // So we compute RustFFT output then apply the same scale for apples-to-apples.
            let mut planner = FftPlanner::<f64>::new();
            let rustfft_plan = planner.plan_fft_forward(n);
            rustfft_plan.process(&mut theirs);
            let scale = 1.0 / (n as f64).sqrt();
            for x in theirs.iter_mut() {
                *x = *x * scale;
            }

            super::fft(&mut ours);
            let theirs_back = from_num_complex(&theirs);

            assert!(approx_eq(&ours, &theirs_back, 1e-9), "Mismatch at n={}", n);
        }
    }
}
