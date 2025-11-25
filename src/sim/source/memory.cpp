#include "sim/memory.hpp"

#include <cassert>
#include <cstring>
#include <cstdint>

#include <spdlog/spdlog.h>

#include "isa/isa_defs.hpp"
#include "sim/memory_segm.hpp"

namespace sim { 

namespace {

bool IsAddrInSegment(isa::Address addr, const MemorySegm& segm);

} // namespace

uint32_t Memory::ReadU32(isa::Address addr) {
    spdlog::trace("Memory u32 read: addr {}", addr);

    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        uint32_t data = 0;
        std::memcpy(&data, segm.data.get() + offset, sizeof(data));

        return data;
    }
        
    assert(!"no segment matched address");
    return 0;
}

uint16_t Memory::ReadU16(isa::Address addr) {
    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        uint16_t data = 0;
        std::memcpy(&data, segm.data.get() + offset, sizeof(data));

        return data;
    }
        
    assert(!"no segment matched address");
    return 0;
}

uint8_t Memory::ReadU8(isa::Address addr) {
    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        uint8_t data = 0;
        std::memcpy(&data, segm.data.get() + offset, sizeof(data));

        return data;
    }
        
    assert(!"no segment matched address");
    return 0;
}

void Memory::StoreU32(isa::Address addr, uint32_t val) {
    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        std::memcpy(segm.data.get() + offset, &val, sizeof(val));

        return ;
    }
       
    assert(!"no segment matched address");
}

void Memory::StoreU16(isa::Address addr, uint16_t val) {
    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        std::memcpy(segm.data.get() + offset, &val, sizeof(val));

        return ;
    }
       
    assert(!"no segment matched address");
}

void Memory::StoreU8(isa::Address addr, uint8_t val) {
    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        std::memcpy(segm.data.get() + offset, &val, sizeof(val));

        return ;
    }
       
    assert(!"no segment matched address");
}

namespace {

bool IsAddrInSegment(isa::Address addr, const MemorySegm& segm) {
    return ((segm.start_addr <= addr) && (addr <= segm.end_addr));
}

} // namespace

} // namespace sim
