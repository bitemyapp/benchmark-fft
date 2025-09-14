#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "complex.hpp"
#include "cooley_tukey.hpp"

double round_n(double n) { return round(n * 100.0) / 100.0; }

std::vector<Complex> generate_inputs(int len) {
  std::vector<Complex> res;
  res.reserve(len);
  for (int i = 0; i < len; i++) {
    double theta = (double)i / len * PI;
    double re = 1.0 * cos(10.0 * theta) + 0.5 * cos(25.0 * theta);
    double im = 1.0 * sin(10.0 * theta) + 0.5 * sin(25.0 * theta);
    res.emplace_back(round_n(re), round_n(im));
  }
  return res;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <size> [verify_file]" << std::endl;
    return 1;
  }

  int size = std::stoi(argv[1]);
  std::vector<Complex> signals = generate_inputs(1 << size);

  auto start = std::chrono::high_resolution_clock::now();
  fft(signals);
  auto end = std::chrono::high_resolution_clock::now();

  if (argc > 2) {
    std::ifstream verify_file(argv[2]);
    if (!verify_file.is_open()) {
      std::cerr << "Error opening verification file: " << argv[2] << std::endl;
      return 1;
    }
    std::string line;
    int i = 0;
    while (std::getline(verify_file, line)) {
      std::stringstream ss(line);
      std::string re_str, im_str;
      std::getline(ss, re_str, ',');
      std::getline(ss, im_str);
      double re = std::stod(re_str);
      double im = std::stod(im_str);
      if (std::abs(signals[i].real - re) > 1e-9 ||
          std::abs(signals[i].imag - im) > 1e-9) {
        std::cerr << "Verification failed at index " << i << std::endl;
        std::cerr << "Expected: " << re << "," << im << std::endl;
        std::cerr << "Got: " << signals[i].real << "," << signals[i].imag
                  << std::endl;
        return 1;
      }
      i++;
    }
  } else {
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "execution time: " << std::fixed << std::setprecision(3)
              << elapsed.count() << " ms" << std::endl;
  }

  return 0;
}
