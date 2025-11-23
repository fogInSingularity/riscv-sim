#ifndef SIM_DEFS_HPP_
#define SIM_DEFS_HPP_

#include <cstdint>
#include <cstddef>

namespace sim {
    
using MemByte = uint8_t;
using MemSize = uint32_t;
using Register = uint32_t;
using Address = uint32_t;
using UndecodedInsn = uint32_t;

constexpr size_t kNumXRegisters = 32;
constexpr size_t kNumFRegisters = 32;

};

#endif // SIM_DEFS_HPP_
