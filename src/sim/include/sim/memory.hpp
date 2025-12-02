#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <cstdint>
#include <cstddef>
#include <vector>
#include <utility>

#include "sim/memory_segm.hpp"
#include "isa/isa_defs.hpp"

namespace sim {

// FIXME check perms
// FIXME check for alignment
class Memory {
  private:
    std::vector<MemorySegm> memory_;
  public:
    explicit Memory(std::vector<MemorySegm> memory)
        : memory_{std::move(memory)} {}
    
    isa::UndecodedInsn Fetch(isa::Address addr) {
        return LoadU32(addr);      
    }
    
    uint32_t LoadU32(isa::Address addr);
    uint16_t LoadU16(isa::Address addr);
    uint8_t LoadU8(isa::Address addr);

    void StoreU32(isa::Address addr, uint32_t val);
    void StoreU16(isa::Address addr, uint16_t val);
    void StoreU8(isa::Address addr, uint8_t val);

    void CopyGuestToHost(isa::Address from_addr, isa::MemByte* to_buf, size_t size); // write syscall
    void CopyHostToGuest(isa::Address to_addr, const isa::MemByte* from_buf, size_t size); // read syscall
    
    void AddSegment(MemorySegm segm) {
        memory_.push_back(std::move(segm));
    }
};

} // namespace sim

#endif // MEMORY_HPP_
