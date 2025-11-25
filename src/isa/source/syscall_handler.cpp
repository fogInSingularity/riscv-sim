#include "isa/syscall_handler.hpp"

#include <cassert>

#include <spdlog/spdlog.h>

#include "helpers/common.hpp"
#include "isa/isa_defs.hpp"

namespace isa {

void SyscallHandler(sim::RVSim* sim) {
    assert(sim != nullptr);

    auto syscall_id = sim->GetXReg(hlp::FromEnum(
        XRegAlias::a7
    ));
    
    switch (syscall_id) {
        case (hlp::FromEnum(SyscallId::kExit)):
            sim->ShouldStop() = true;
            // because return value of sim is a0 and a0 is argument of exit -> do nothing with xregs
            spdlog::info(
                "Syscall exit had been called, terminating: ip {}, status {}", 
                sim->Ip(), 
                sim->GetXReg(hlp::FromEnum(XRegAlias::a0))
            );
            return ;
        default:
            spdlog::error("unknown syscall id: {}", syscall_id);
            assert(!"unknown syscall id");
    }
}

} // namespace isa
