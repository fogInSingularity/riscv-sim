#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <cstdint>
#include <vector>
#include <utility>

#include "sim/memory_segm.hpp"
#include "sim/sim_defs.hpp"

namespace sim {

class Memory {
  private:
    std::vector<MemorySegm> memory_;
  public:
    explicit Memory(std::vector<MemorySegm> memory)
        : memory_{std::move(memory)} {}
    
    UndecodedInsn Fetch(Address addr) {
        return ReadU32(addr);      
    }
    
    uint32_t ReadU32(Address addr);
};

} // namespace sim

#endif // MEMORY_HPP_
