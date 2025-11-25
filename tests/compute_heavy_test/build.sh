#!/bin/bash

riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    $1 -o $1.s -S -O2

riscv64-unknown-elf-as \
    -march=rv32im \
    -mabi=ilp32 \
    -o $1.o $1.s

riscv64-unknown-elf-as \
    -march=rv32im \
    -mabi=ilp32 \
    -o api.o api.s

riscv64-unknown-elf-ld \
    -m elf32lriscv \
    -o $1.x \
    $1.o api.o
