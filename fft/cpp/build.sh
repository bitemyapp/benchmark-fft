#!/bin/bash
mkdir -p build
g++ -O3 -std=c++17 -march=native -funroll-loops -flto -DNDEBUG main.cpp -o build/fft_bench -lm
