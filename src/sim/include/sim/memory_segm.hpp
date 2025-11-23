#ifndef MEMORY_SEGM_HPP
#define MEMORY_SEGM_HPP

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "sim/sim_defs.hpp"

namespace sim {

struct MemorySegm {
    Address start_addr;
    Address end_addr;
    std::unique_ptr<MemByte[]> data;

    // permissions
    bool r_permission;
    bool w_permission;
    bool x_permission;
};

} // namespace sim

#endif // MEMORY_SEGM_HPP
