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
#include "sim/program_args.hpp"

namespace sim {

class RVSim {
  private:
    using XRegArrayT = std::array<isa::Register, isa::kNumXRegisters>;
    using FRegArrayT = std::array<isa::FRegister, isa::kNumFRegisters>;
   
    Memory memory_;
    isa::Address ip_;
    bool should_stop_;
    XRegArrayT xregs_;
    FRegArrayT fregs_;
    isa::Register fcsr_; // float csr
  public:
    explicit RVSim(const ParsedElf& parsed_elf, const ProgramArgs& prog_args)
        : memory_{parsed_elf.mem}, 
        ip_{parsed_elf.entry_point}, 
        should_stop_{false},
        xregs_{0},
        fregs_{0},
        fcsr_{0}
    {
        // add stack 
        auto stack_segm = MemorySegm{
            isa::kStackBase, 
            isa::kStackTop, // top not included
            decltype(MemorySegm::data)(isa::kStackSize),
            true,
            true,
            false
        };  

        auto argv_size = prog_args.SizeOfArgv();
        
        auto argv_segm = MemorySegm{
            isa::kArgvBase,
            isa::kArgvBase + argv_size + decltype(isa::kArgvBase){sizeof(isa::Register)}, 
            decltype(MemorySegm::data)(argv_size),
            true,
            false,
            false,
        };
        auto argc = prog_args.Argc();
        auto offset_table = prog_args.CopyArgv(argv_segm.data.data());

        isa::Address argv_fill_start = isa::kStackTop - sizeof(isa::Register) * (argc + 2 /*argc + argv == NULL*/);
        auto fill_addr = stack_segm.data.data() + argv_fill_start - isa::kStackBase;
        std::memcpy(fill_addr, &argc, sizeof(isa::Register));
        fill_addr += sizeof(isa::Register);

        for (int i = 0; i < prog_args.Argc(); i++) {
            auto offset = isa::kArgvBase + offset_table[i];
            std::memcpy(fill_addr, &offset, sizeof(isa::Register));
            fill_addr += sizeof(isa::Register);
        }

        isa::Register null_ptr = 0;
        std::memcpy(fill_addr, &null_ptr, sizeof(null_ptr));

        memory_.AddSegment(stack_segm);
        memory_.AddSegment(argv_segm);

        xregs_[hlp::FromEnum(isa::XRegAlias::gp)] = parsed_elf.global_ptr;
        xregs_[hlp::FromEnum(isa::XRegAlias::sp)] = argv_fill_start;
    }

    int Execute();

    bool& ShouldStop() {
        return should_stop_;
    }

    void SetXReg(size_t index, isa::Register value) {
        assert(index < isa::kNumXRegisters);
        
        SPDLOG_TRACE("set x reg: index {}, value {}", index, value);

        if (index == hlp::FromEnum(isa::XRegAlias::zero)) { return; }

        xregs_[index] = value;
    }

    void SetXReg(isa::XRegAlias reg_alias, isa::Register value) {
        return SetXReg(hlp::FromEnum(reg_alias), value);
    }
    
    isa::Register GetXReg(size_t index) {
        assert(index < isa::kNumXRegisters);

        if (index == hlp::FromEnum(isa::XRegAlias::zero)) { return 0; }

        return xregs_[index];
    }

    isa::Register GetXReg(isa::XRegAlias reg_alias) {
        return GetXReg(hlp::FromEnum(reg_alias));
    }

    void SetFReg(size_t index, isa::FRegister value) {
        assert(index < isa::kNumFRegisters);
        SPDLOG_TRACE("set f reg: index {}, value {:#x}", index, value.v);

        fregs_[index] = value;
    }

    isa::FRegister GetFReg(size_t index) {
        assert(index < isa::kNumFRegisters);

        auto fval = fregs_[index];
        SPDLOG_TRACE("get f reg: index {}, value {:#x}", index, fval.v);
        return fval;
    }

    void SetCSR(size_t index, isa::Register value) {
        SPDLOG_TRACE("set csr called: index {}, value {}", index, value);

        switch (hlp::ToEnum<isa::CSR>(index)) {
            case isa::CSR::fflags:
                // FIXME make function
                fcsr_ = (fcsr_ & ~0x1fu) | (value & 0x1fu);
                break;
            case isa::CSR::frm:
                fcsr_ = (fcsr_ & ~(0x7u << 5)) | ((value & 0x7u) << 5);
                break;
            case isa::CSR::fcsr:
                fcsr_ = value & 0xffu;
                break;
            default:
                assert(!"unknown csr");
        }
    }

    void SetCSR(isa::CSR csr, isa::Register value) {
        return SetCSR(hlp::FromEnum(csr), value);
    }

    isa::Register GetCSR(size_t index) {
        SPDLOG_TRACE("get csr called: index {}", index);
        
        switch (hlp::ToEnum<isa::CSR>(index)) {
            case isa::CSR::fflags:
                return fcsr_ & 0x1fu;
            case isa::CSR::frm:
                return (fcsr_ >> 5) & 0x7u;
            case isa::CSR::fcsr:
                return fcsr_ & 0xffu;
            default:
                assert(!"unknown csr");
                return 0;
        }
    }

    isa::Register GetCSR(isa::CSR csr) {
        return GetCSR(hlp::FromEnum(csr));
    }

    isa::RoundingMode GetRM(isa::Register rm) {
        if (hlp::ToEnum<isa::RoundingMode>(rm) == isa::RoundingMode::DYN) {
            return hlp::ToEnum<isa::RoundingMode>(GetCSR(isa::CSR::frm));
        } else {
            return hlp::ToEnum<isa::RoundingMode>(rm);
        }
    } 

    auto& Ip() {
        return ip_;
    }

    void Step() {
        ip_ += isa::kStepSize;
    }

    auto& Mem() {
        return memory_;
    }

    void Trace() {
        SPDLOG_TRACE("trace sim:");
        for (size_t i = 0; i < isa::kNumXRegisters; i++) {
            SPDLOG_TRACE("Reg {}: {}", i, xregs_[i]);
        }
    }
};

} // namespace sim

#endif // RV_SIM_HPP_
