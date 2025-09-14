#!/bin/bash

mkdir -p build
gcc -O3 -march=native -funroll-loops -flto -DNDEBUG \
	-Wall -Wextra -o build/fft_bench \
	fft.c \
	main.c \
	-lm
