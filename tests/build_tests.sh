#!/bin/bash

mkdir -p build

riscv64-unknown-elf-as \
    -march=rv32imf_zbb \
    -mabi=ilp32f \
    -o build/$1.o \
    $1

riscv64-unknown-elf-ld \
    -m elf32lriscv \
    -o build/$1.x \
    build/$1.o
