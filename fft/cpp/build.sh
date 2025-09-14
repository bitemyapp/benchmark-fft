#!/bin/bash
mkdir -p build
g++ -O3 -std=c++17 main.cpp -o build/fft_bench
