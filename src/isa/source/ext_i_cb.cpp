#include "isa/ext_i_cb.hpp"

#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>

#include "helpers/trace_calls.hpp"
#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"
#include "isa/rv_insn.hpp"
#include "isa/syscall_handler.hpp"
#include "isa/isa_hlp.hpp"

namespace isa {

namespace {

constexpr uint32_t ArithmRightShift(uint32_t x, unsigned shift) {
    constexpr unsigned num_bits = sizeof(x) * CHAR_BIT;
    constexpr uint32_t one = uint32_t{1};
    constexpr unsigned sign_bit = num_bits - 1;
    constexpr uint32_t sign_bit_mask = one << sign_bit;
    constexpr uint32_t all_ones = std::numeric_limits<uint32_t>::max();
    constexpr uint32_t zero = uint32_t{0};

    if (shift == 0) { return x; }
    
    if (shift >= num_bits) {
        return !IsBitSet(x, sign_bit) ? all_ones : zero;
    }

    uint32_t logical = x >> shift;

    if (!IsBitSet(x, sign_bit)) { return logical; }

    uint32_t fill_mask = all_ones << (num_bits - shift);
    return logical | fill_mask;
}

template <typename BinOpFunc>
void ExecuteRRRBinOp(sim::RVSim* sim, isa::UndecodedInsn raw_insn, BinOpFunc bin_op) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    sim->SetXReg(
        insn.Rd(),
        bin_op(
            sim->GetXReg(insn.Rs1()), 
            sim->GetXReg(insn.Rs2())
        )
    );

    sim->Ip() += kStepSize;
}

template <typename BinOpFunc>
void ExecuteRRIBinOp(sim::RVSim* sim, isa::UndecodedInsn raw_insn, BinOpFunc bin_op) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    sim->SetXReg(
        insn.Rd(),
        bin_op(
            sim->GetXReg(insn.Rs1()), 
            insn.ImmI()
        )
    );

    sim->Ip() += kStepSize;
}

template <typename BinOpFunc>
void ExecuteRRIShift(sim::RVSim* sim, isa::UndecodedInsn raw_insn, BinOpFunc bin_op) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    sim->SetXReg(
        insn.Rd(),
        bin_op(
            sim->GetXReg(insn.Rs1()), 
            insn.Shamt()
        )
    );

    sim->Ip() += kStepSize;
}

template <typename BranchPredFunc>
void ExecuteBranch(sim::RVSim* sim, isa::UndecodedInsn raw_insn, BranchPredFunc pred) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    
    bool should_branch = pred(
        sim->GetXReg(insn.Rs1()),
        sim->GetXReg(insn.Rs2())
    );

    if (should_branch) {
        sim->Ip() += insn.ImmB();
    } else {
        sim->Ip() += kStepSize;
    }
}

} // namespace

void CallbackLUI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};
    sim->SetXReg(
        insn.Rd(),
        insn.ImmU()
    );

    sim->Ip() += kStepSize;
}

void CallbackAUIPC(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    sim->SetXReg(
        insn.Rd(),
        sim->Ip() + insn.ImmU()
    );

    sim->Ip() += kStepSize;
}

void CallbackJAL(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
 
    RvInsn insn{raw_insn};
    sim->SetXReg(insn.Rd(), sim->Ip() + kStepSize);

    sim->Ip() += insn.ImmJ();
}

void CallbackJALR(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    auto hold_pc = sim->Ip() + kStepSize;

    sim->Ip() = 
        (sim->GetXReg(insn.Rs1()) + insn.ImmI()) 
        & ~Register{1};

    sim->SetXReg(insn.Rd(), hold_pc);
}

void CallbackBEQ(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return a == b; });
}

void CallbackBNE(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return a != b; });
}

void CallbackBLT(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return RegToIReg(a) < RegToIReg(b); });
}

void CallbackBGE(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return RegToIReg(a) >= RegToIReg(b); });
}

void CallbackBLTU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return a < b; });
}

void CallbackBGEU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return a >= b; });
}

void CallbackLB(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().LoadU8(addr);

    sim->SetXReg(
        insn.Rd(),
        SignExt<uint32_t>(mem_val)
    );

    sim->Ip() += kStepSize;
}

void CallbackLH(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().LoadU16(addr);

    sim->SetXReg(
        insn.Rd(),
        SignExt<uint32_t>(mem_val)
    );

    sim->Ip() += kStepSize;
}

void CallbackLW(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().LoadU32(addr);

    sim->SetXReg(
        insn.Rd(),
        mem_val
    );

    sim->Ip() += kStepSize;
}

void CallbackLBU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().LoadU8(addr);

    sim->SetXReg(
        insn.Rd(),
        mem_val
    );
    
    sim->Ip() += kStepSize;
}

void CallbackLHU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().LoadU16(addr);

    sim->SetXReg(
        insn.Rd(),
        mem_val
    );

    sim->Ip() += kStepSize;
}

void CallbackSB(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
 
    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmS();
    auto val = TruncHigh<uint8_t>(
        sim->GetXReg(insn.Rs2())
    );

    sim->Mem().StoreU8(addr, val);

    sim->Ip() += kStepSize;
}

void CallbackSH(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmS();
    auto val = TruncHigh<uint16_t>(
        sim->GetXReg(insn.Rs2())
    );

    sim->Mem().StoreU16(addr, val);

    sim->Ip() += kStepSize;
}

void CallbackSW(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmS();
    auto val = sim->GetXReg(insn.Rs2());

    sim->Mem().StoreU32(addr, val);

    sim->Ip() += kStepSize;
}

void CallbackADDI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRIBinOp(sim, raw_insn, [](Register a, Register b) { return a + b; });
}

void CallbackSLTI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
     
    RvInsn insn{raw_insn};
    auto irs1 = RegToIReg(
        sim->GetXReg(insn.Rs1())
    );
    auto imm = insn.ImmI();

    bool is_less_then = irs1 < imm;

    sim->SetXReg(
        insn.Rd(),
        is_less_then ? 1 : 0
    );

    sim->Ip() += kStepSize;
}

void CallbackSLTIU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    bool is_less_then = sim->GetXReg(insn.Rs1()) < IRegToReg(insn.ImmI());

    sim->SetXReg(
        insn.Rd(),
        is_less_then ? 1 : 0
    );

    sim->Ip() += kStepSize;
}

void CallbackXORI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRIBinOp(sim, raw_insn, [](Register a, Register b) { return a ^ b; });
}

void CallbackORI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRIBinOp(sim, raw_insn, [](Register a, Register b) { return a | b; });
}

void CallbackANDI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRIBinOp(sim, raw_insn, [](Register a, Register b) { return a & b; });
}

void CallbackSLLI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRIShift(sim, raw_insn, [](Register a, Register b) { return a << b; });
}

void CallbackSRLI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRIShift(sim, raw_insn, [](Register a, Register b) { return a >> b; });
}

void CallbackSRAI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRIShift(
        sim, 
        raw_insn, 
        [](Register a, Register b) { return ArithmRightShift(a, b); }
    );
}

void CallbackADD(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a + b; });
}

void CallbackSUB(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a - b; });
}

void CallbackSLL(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a << b; });
}

void CallbackSLT(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    auto irs1 = RegToIReg(sim->GetXReg(insn.Rs1()));
    auto irs2 = RegToIReg(sim->GetXReg(insn.Rs2()));

    bool is_less_then = irs1 < irs2;

    sim->SetXReg(
        insn.Rd(),
        is_less_then ? 1 : 0
    );

    sim->Ip() += kStepSize;
}

void CallbackSLTU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};
    bool is_less_then = sim->GetXReg(insn.Rs1()) < sim->GetXReg(insn.Rs2());

    sim->SetXReg(
        insn.Rd(),
        is_less_then ? 1 : 0
    );

    sim->Ip() += kStepSize;
}

void CallbackXOR(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a ^ b; });
}

void CallbackSRL(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a >> b; });
}

void CallbackSRA(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(
        sim, 
        raw_insn, 
        [](Register a, Register b) { return ArithmRightShift(a, b & (kXlen - 1)); }
    );
}

void CallbackOR(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a | b; });
}

void CallbackAND(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a & b; });
}

void CallbackFENCE(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    spdlog::info("FENCE instruction encountered");

    sim->Ip() += kStepSize;
}

void CallbackFENCE_I(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    spdlog::info("FENCE_I instruction encountered");

    sim->Ip() += kStepSize;
}

void CallbackECALL(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    SyscallHandler(sim);

    sim->Ip() += kStepSize;
}

void CallbackEBREAK(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    spdlog::info("EBREAK instruction encountered");    

    sim->Ip() += kStepSize;
}

} // namespace isa
