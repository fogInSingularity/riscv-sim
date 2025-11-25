#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <cstdint>
#include <vector>
#include <utility>

#include "sim/memory_segm.hpp"
#include "isa/isa_defs.hpp"

namespace sim {

class Memory {
  private:
    std::vector<MemorySegm> memory_;
  public:
    explicit Memory(std::vector<MemorySegm> memory)
        : memory_{std::move(memory)} {}
    
    isa::UndecodedInsn Fetch(isa::Address addr) {
        return ReadU32(addr);      
    }
    
    uint32_t ReadU32(isa::Address addr);
    uint16_t ReadU16(isa::Address addr);
    uint8_t ReadU8(isa::Address addr);

    void StoreU32(isa::Address addr, uint32_t val);
    void StoreU16(isa::Address addr, uint16_t val);
    void StoreU8(isa::Address addr, uint8_t val);
};

} // namespace sim

#endif // MEMORY_HPP_
