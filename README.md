# riscv-sim
risc-v simulator homework for compiler class. Implementation supports rv32imf_zbb(+zicsr) arch.

## Prerequisites

- C++ compiler (with C++20 support)
- CMake (3.21+)

## Build

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Usage 

```bash
./build/riscv-sim.x <elf_bin> <args for elf_bin...>
```
