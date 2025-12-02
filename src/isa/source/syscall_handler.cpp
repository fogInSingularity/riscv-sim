#include "isa/syscall_handler.hpp"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <vector>

#include <unistd.h> // read, write

#include <spdlog/spdlog.h>

#include "helpers/common.hpp"
#include "isa/isa_defs.hpp"

namespace isa {

void SyscallHandler(sim::RVSim* sim) {
    assert(sim != nullptr);

    auto syscall_id = sim->GetXReg(XRegAlias::a7);
    
    switch (syscall_id) {
        case (hlp::FromEnum(SyscallId::kExit)):
            sim->ShouldStop() = true;
            // because return value of sim is a0 and a0 is argument of exit -> do nothing with xregs
            spdlog::info(
                "syscall exit had been called, terminating: ip {}, status {}", 
                sim->Ip(), 
                sim->GetXReg(XRegAlias::a0)
            );
            return ;
        case (hlp::FromEnum(SyscallId::kRead)): {
            spdlog::info("syscall read had been called");

            int fd = sim->GetXReg(XRegAlias::a0);
            auto guest_buf_ptr = sim->GetXReg(XRegAlias::a1);
            auto size = sim->GetXReg(XRegAlias::a2);

            std::vector<MemByte> buf(size);
            auto rc = read(fd, buf.data(), size);
            if (rc >= 0) {
                sim->Mem().CopyHostToGuest(guest_buf_ptr, buf.data(), rc);
            } else {
                spdlog::error("read syscall failed: fd {}, error {}", fd, strerror(errno));
            }
            
            sim->SetXReg(XRegAlias::a0, rc);

            return ;
        }
        case (hlp::FromEnum(SyscallId::kWrite)): {
            spdlog::info("syscall write had been called");

            int fd = sim->GetXReg(XRegAlias::a0);
            auto guest_buf_ptr = sim->GetXReg(XRegAlias::a1);
            auto size = sim->GetXReg(XRegAlias::a2);

            std::vector<MemByte> buf(size);
            sim->Mem().CopyGuestToHost(guest_buf_ptr, buf.data(), size);

            auto wc = write(fd, buf.data(), size);
            if (wc < 0) {
                spdlog::error("write syscall failed: fd {}, error {}", fd, strerror(errno));
            }

            sim->SetXReg(XRegAlias::a0, wc);

            return ;
        }
        default:
            spdlog::error("unknown syscall id: {}", syscall_id);
            assert(!"unknown syscall id");
    }
}

} // namespace isa
