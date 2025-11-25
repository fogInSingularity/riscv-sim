#ifndef SIM_DEFS_HPP_
#define SIM_DEFS_HPP_

#include <cstdint>
#include <cstddef>

namespace isa {
    
using MemByte = uint8_t;
using MemSize = uint32_t;
using Register = uint32_t;
using IRegister = int32_t;
using Address = uint32_t;
using UndecodedInsn = uint32_t;
using InsnField = uint32_t;

constexpr size_t kStepSize = sizeof(UndecodedInsn);

constexpr size_t kNumXRegisters = 32;
constexpr size_t kNumFRegisters = 32;

// register aliases
// https://riscv.org/wp-content/uploads/2024/12/riscv-calling.pdf
enum class XRegAlias : uint32_t {
    zero = 0,
    ra = 1, // return address
    sp = 2, // stack pointer
    gp = 3, // global pointer
    tp = 4, // thread pointer
    t0 = 5, // temp
    t1 = 6,
    t2 = 7,
    fp = 8, // frame pointer
    s0 = 8, // saved reg
    s1 = 9,
    a0 = 10, // function argument / return value
    a1 = 11,
    a2 = 12, // function argument
    a3 = 13,
    a4 = 14,
    a5 = 15,
    a6 = 16,
    a7 = 17, // also syscall number
    s2 = 18, // saved reg
    s3 = 19,
    s4 = 20,
    s5 = 21,
    s6 = 22,
    s7 = 23,
    s8 = 24,
    s9 = 25,
    s10 = 26,
    s11 = 27,
    t3 = 28, // temp
    t4 = 29,
    t5 = 30,
    t6 = 31,
};

enum class FRegAlias : uint32_t {
    ft0 = 0, // fp temp
    ft1 = 1,
    ft2 = 2,
    ft3 = 3,
    ft4 = 4,
    ft5 = 5,
    ft6 = 6,
    ft7 = 7,
    fs0 = 8, // fp saved reg
    fs1 = 9,
    fa0 = 10, // fp function argument / return value
    fa1 = 11,     
    fa2 = 12, // fp argument
    fa3 = 13,
    fa4 = 14,
    fa5 = 15,
    fa6 = 16,
    fa7 = 17, 
    fs2 = 18, // fp saved reg
    fs3 = 19,
    fs4 = 20,
    fs5 = 21,
    fs6 = 22,
    fs7 = 23,
    fs8 = 24,
    fs9 = 25,
    fs10 = 26,
    fs11 = 27,
    ft8 = 28, // fp temp
    ft9 = 29,
    ft10 = 30,
    ft11 = 31,
};

} // namespace isa

#endif // SIM_DEFS_HPP_
