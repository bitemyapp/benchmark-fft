use criterion::{criterion_group, criterion_main, BatchSize, BenchmarkId, Criterion, Throughput};
use fft::{fft, fft_rayon, Complex};
use std::f64::consts::PI;

fn generate_inputs(len: usize) -> Vec<Complex> {
    let mut res = Vec::with_capacity(len);
    for i in 0..len {
        let theta = i as f64 / len as f64 * PI;
        let re = 1.0 * (10.0 * theta).cos() + 0.5 * (25.0 * theta).cos();
        let im = 1.0 * (10.0 * theta).sin() + 0.5 * (25.0 * theta).sin();
        res.push(Complex::new(re, im));
    }
    res
}

fn bench_fft(c: &mut Criterion) {
    let mut group = c.benchmark_group("fft");
    // Mirror bench_runner inputs: 2^18, 2^20, 2^22
    let exp_sizes = [18usize, 20, 22];
    // Align with bench_runner default runs; Criterion samples differently, but we can keep it modest
    group.sample_size(10);

    for &e in &exp_sizes {
        let n = 1usize << e;
        group.throughput(Throughput::Elements(n as u64));
        group.bench_with_input(BenchmarkId::new("fft", format!("2^{e}")), &n, |b, &n| {
            b.iter_batched(
                || generate_inputs(n),
                |mut data| {
                    fft(&mut data);
                },
                BatchSize::LargeInput,
            );
        });
        group.bench_with_input(
            BenchmarkId::new("fft_rayon", format!("2^{e}")),
            &n,
            |b, &n| {
                b.iter_batched(
                    || generate_inputs(n),
                    |mut data| {
                        fft_rayon(&mut data);
                    },
                    BatchSize::LargeInput,
                );
            },
        );
    }

    group.finish();
}

criterion_group!(benches, bench_fft);
criterion_main!(benches);
