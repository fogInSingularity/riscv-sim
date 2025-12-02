#ifndef MEMORY_SEGM_HPP
#define MEMORY_SEGM_HPP

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

#include "isa/isa_defs.hpp"

namespace sim {

struct MemorySegm {
    isa::Address start_addr;
    isa::Address end_addr;
    std::vector<isa::MemByte> data;

    // permissions
    bool r_permission;
    bool w_permission;
    bool x_permission;
};

} // namespace sim

#endif // MEMORY_SEGM_HPP
