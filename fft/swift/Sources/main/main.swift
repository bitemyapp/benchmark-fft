@preconcurrency import Glibc
import FFT
import Foundation

func round(_ n: Double) -> Double {
  // precision = 2
  Foundation.round(n * 100.0) / 100.0
}

func generateInputs(len: Int) -> [Complex] {
  var res = [Complex]()
  res.reserveCapacity(len)
  for i in 0..<len {
    let theta = Double(i) / Double(len) * Double.pi
    let re = 1.0 * cos(10.0 * theta) + 0.5 * cos(25.0 * theta)
    let im = 1.0 * sin(10.0 * theta) + 0.5 * sin(25.0 * theta)
    res.append(Complex(real: round(re), imag: round(im)))
  }
  return res
}

let args = CommandLine.arguments
guard args.count > 1, let size = Int(args[1]) else {
  fputs("usage: \(args.first ?? "prog") <size>\n", stderr)
  exit(1)
}
let len = 1 << size

var signals = generateInputs(len: len)

// Measure with a monotonic clock and report milliseconds like Rust
let start = DispatchTime.now()
fft(&signals)
let end = DispatchTime.now()

let elapsedNs = end.uptimeNanoseconds - start.uptimeNanoseconds
let elapsedMs = Double(elapsedNs) / 1_000_000.0
print(String(format: "execution time: %.3f ms", elapsedMs))

// var signals = generateInputs(len: 16384)
// fft(&signals)
// for signal in signals {
//   print("\(round(signal.real)),\(round(signal.imag))")
// }
