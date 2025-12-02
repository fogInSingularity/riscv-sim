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

uint32_t Memory::LoadU32(isa::Address addr) {
    SPDLOG_TRACE("Memory u32 read: addr {}", addr);

    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        uint32_t data = 0;
        std::memcpy(&data, segm.data.data() + offset, sizeof(data));

        return data;
    }
        
    assert(!"no segment matched address");
    return 0;
}

uint16_t Memory::LoadU16(isa::Address addr) {
    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        uint16_t data = 0;
        std::memcpy(&data, segm.data.data() + offset, sizeof(data));

        return data;
    }
        
    assert(!"no segment matched address");
    return 0;
}

uint8_t Memory::LoadU8(isa::Address addr) {
    for (const auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        uint8_t data = 0;
        std::memcpy(&data, segm.data.data() + offset, sizeof(data));

        return data;
    }
        
    assert(!"no segment matched address");
    return 0;
}

void Memory::StoreU32(isa::Address addr, uint32_t val) {
    for (auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        std::memcpy(segm.data.data() + offset, &val, sizeof(val));

        return ;
    }
       
    assert(!"no segment matched address");
}

void Memory::StoreU16(isa::Address addr, uint16_t val) {
    for (auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        std::memcpy(segm.data.data() + offset, &val, sizeof(val));

        return ;
    }
       
    assert(!"no segment matched address");
}

void Memory::StoreU8(isa::Address addr, uint8_t val) {
    for (auto& segm: memory_) {
        if (!IsAddrInSegment(addr, segm)) { continue; }

        isa::Address offset = addr - segm.start_addr;

        std::memcpy(segm.data.data() + offset, &val, sizeof(val));

        return ;
    }
       
    assert(!"no segment matched address");
}

void Memory::CopyGuestToHost(isa::Address from_addr, isa::MemByte* to_buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        to_buf[i] = LoadU8(from_addr + i);
    }
}

void Memory::CopyHostToGuest(isa::Address to_addr, const isa::MemByte* from_buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        StoreU8(to_addr + i, from_buf[i]);
    }
}

namespace {

// memory segment [begin, end)
bool IsAddrInSegment(isa::Address addr, const MemorySegm& segm) {
    return ((segm.start_addr <= addr) && (addr < segm.end_addr));
}

} // namespace

} // namespace sim
