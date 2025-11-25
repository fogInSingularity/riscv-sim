#ifndef SYSCALL_HANDLER_HPP_
#define SYSCALL_HANDLER_HPP_

#include "sim/rv_sim.hpp"

namespace isa {

void SyscallHandler(sim::RVSim* sim);

enum class SyscallId {
    kRead = 63,
    kWrite = 64,
    kExit = 93,
};

} // namespace isa

#endif // SYSCALL_HANDLER_HPP_
