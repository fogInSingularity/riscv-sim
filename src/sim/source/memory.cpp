#include "sim/memory.hpp"

#include <cassert>
#include <cstring>
#include <cstdint>

#include "sim/sim_defs.hpp"
#include "sim/memory_segm.hpp"

namespace sim { 

namespace {

bool IsAddrInSegment(Address addr, const MemorySegm& segm);

} // namespace

uint32_t Memory::ReadU32(Address addr) {
    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        Address offset = addr - segm.start_addr;

        uint32_t data = 0;
        std::memcpy(&data, segm.data.get() + offset, sizeof(data));

        return data;
    }
        
    assert(!"no segment matched address");
    return 0;
}

namespace {

bool IsAddrInSegment(Address addr, const MemorySegm& segm) {
    return ((segm.start_addr <= addr) && (addr <= segm.end_addr));
}

} // namespace

} // namespace sim
