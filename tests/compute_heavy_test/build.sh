#!/bin/bash

riscv64-unknown-elf-gcc \
    -march=rv32imf_zbb \
    -mabi=ilp32f \
    -nostdlib \
    -ffreestanding \
    $1 -o $1.s -S -O2

riscv64-unknown-elf-as \
    -march=rv32imf_zbb \
    -mabi=ilp32f \
    -o $1.o $1.s

riscv64-unknown-elf-as \
    -march=rv32imf_zbb \
    -mabi=ilp32f \
    -o api.o api.s

riscv64-unknown-elf-ld \
    -m elf32lriscv \
    -o $1.x \
    $1.o api.o
