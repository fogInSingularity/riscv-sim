#ifndef RV_SIM_HPP_
#define RV_SIM_HPP_

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include <array>
#include <cassert>

#include <spdlog/spdlog.h>

#include "helpers/common.hpp"
#include "sim/memory_segm.hpp"
#include "sim/memory.hpp"
#include "isa/isa_defs.hpp"
#include "sim/elf_loader.hpp"

namespace sim {

class RVSim {
  private:
    using XRegArrayT = std::array<isa::Register, isa::kNumXRegisters>;
    using FRegArrayT = std::array<isa::Register, isa::kNumFRegisters>;
   
    Memory memory_;
    isa::Address ip_;
    bool should_stop_;
    XRegArrayT xregs;
    FRegArrayT fregs;
  public:
    explicit RVSim(ParsedElf* parsed_elf)
        : memory_{std::move(parsed_elf->mem)}, 
        ip_{parsed_elf->entry_point}, 
        should_stop_{false},
        xregs{0},
        fregs{0}
    {
        memory_.AddSegment(
            MemorySegm{
                isa::kStackBase, 
                isa::kStackTop, 
                std::make_unique<isa::MemByte[]>(isa::kStackSize),
                true,
                true,
                false
            }  
        );

        xregs[hlp::FromEnum(isa::XRegAlias::gp)] = parsed_elf->global_ptr;
        xregs[hlp::FromEnum(isa::XRegAlias::sp)] = isa::kStackTop - sizeof(isa::Register);
    }

    int Execute();

    bool& ShouldStop() {
        return should_stop_;
    }

    void SetXReg(size_t index, isa::Register value) {
        assert(index < isa::kNumXRegisters);
        
        spdlog::trace("set x reg: index {}, value {}", index, value);

        if (index == hlp::FromEnum(isa::XRegAlias::zero)) { return; }

        xregs[index] = value;
    }

    void SetXReg(isa::XRegAlias reg_alias, isa::Register value) {
        return SetXReg(hlp::FromEnum(reg_alias), value);
    }
    
    isa::Register GetXReg(size_t index) {
        assert(index < isa::kNumXRegisters);

        if (index == hlp::FromEnum(isa::XRegAlias::zero)) { return 0; }

        return xregs[index];
    }

    isa::Register GetXReg(isa::XRegAlias reg_alias) {
        return GetXReg(hlp::FromEnum(reg_alias));
    }

    void SetFReg(size_t index, isa::Register value) {
        assert(index < isa::kNumFRegisters);

        fregs[index] = value;
    }

    isa::Register GetFReg(size_t index) {
        assert(index < isa::kNumFRegisters);

        return fregs[index];
    }

    void SetCSR(size_t index, isa::Register value) {
        spdlog::debug("set csr called: index {}, value {}", index, value);
    }

    isa::Register GetCSR(size_t index) {
        spdlog::debug("set csr called: index {}", index);

        return 0;
    }

    auto& Ip() {
        return ip_;
    }

    auto& Mem() {
        return memory_;
    }

    void Trace() {
        spdlog::trace("trace sim:");
        for (size_t i = 0; i < isa::kNumXRegisters; i++) {
            spdlog::trace("Reg {}: {}", i, xregs[i]);
        }
    }
};

} // namespace sim

#endif // RV_SIM_HPP_
