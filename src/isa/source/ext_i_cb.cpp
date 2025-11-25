#include "isa/ext_i_cb.hpp"

#include <cassert>
#include <climits>
#include <cstdint>
#include <bit>
#include <type_traits>
#include <limits>

#include "helpers/trace_calls.hpp"
#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"
#include "isa/rv_insn.hpp"
#include "isa/syscall_handler.hpp"

namespace isa {

namespace {

IRegister RegToIReg(Register val) {
    return std::bit_cast<IRegister>(val);
}

Register IRegToReg(IRegister val) {
    return std::bit_cast<Register>(val);
}

// FIXME
template <typename ToT, typename FromT>
constexpr ToT TruncHigh(FromT x) {
    static_assert(std::is_integral_v<FromT>, "FromT must be an integral type");
    static_assert(std::is_integral_v<ToT>,   "ToT must be an integral type");
    static_assert(
        sizeof(ToT) < sizeof(FromT),
        "ToT must be strictly smaller than FromT"
    );

    using UnsignedFrom = std::make_unsigned_t<FromT>;
    using UnsignedTo   = std::make_unsigned_t<ToT>;

    constexpr UnsignedTo  to_max   = std::numeric_limits<UnsignedTo>::max();
    constexpr UnsignedFrom mask    = static_cast<UnsignedFrom>(to_max);

    const UnsignedFrom ux = static_cast<UnsignedFrom>(x);
    const UnsignedTo   uy = static_cast<UnsignedTo>(ux & mask);

    return static_cast<ToT>(uy);
}

template <typename ToT, typename FromT>
constexpr ToT SignExtT(FromT x) {
    static_assert(std::is_unsigned_v<FromT>, "FromT must be unsigned");
    static_assert(std::is_unsigned_v<ToT>,   "ToT must be unsigned");

    constexpr unsigned from_bits = sizeof(FromT) * CHAR_BIT;
    constexpr unsigned to_bits   = sizeof(ToT) * CHAR_BIT;

    static_assert(
        from_bits < to_bits,
        "SignExtT expects ToT to be strictly wider than FromT"
    );

    constexpr ToT one = ToT{1};
    constexpr ToT value_mask = (one << from_bits) - one;
    constexpr ToT sign_bit = one << (from_bits - 1);
    constexpr ToT extend_mask = ~value_mask;

    ToT v = static_cast<ToT>(x) & value_mask;

    if (v & sign_bit) {
        v |= extend_mask;  // fill high bits
    }

    return v;
}

constexpr uint32_t ArithmRightShift(uint32_t x, unsigned shift) {
    constexpr unsigned num_bits = sizeof(x) * CHAR_BIT;
    constexpr uint32_t one = uint32_t{1};
    constexpr uint32_t sign_bit_mask = one << (num_bits - 1);
    constexpr uint32_t all_ones = std::numeric_limits<uint32_t>::max();
    constexpr uint32_t zero = uint32_t{0};

    if (shift == 0) { return x; }
    
    if (shift >= num_bits) {
        return (x & sign_bit_mask) 
            ? all_ones 
            : zero;
    }

    uint32_t logical = x >> shift;

    if ((x & sign_bit_mask) == 0) { return logical; }

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

void CallbackLUI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};
    sim->SetXReg(
        insn.Rd(),
        insn.ImmU()
    );

    sim->Ip() += kStepSize;
}

void CallbackAUIPC(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    sim->SetXReg(
        insn.Rd(),
        sim->Ip() + insn.ImmU()
    );

    sim->Ip() += kStepSize;
}

void CallbackJAL(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
 
    RvInsn insn{raw_insn};
    sim->SetXReg(insn.Rd(), sim->Ip() + kStepSize);

    sim->Ip() += insn.ImmJ();
}

void CallbackJALR(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};
    auto hold_pc = sim->Ip() + kStepSize;

    sim->Ip() = 
        (sim->GetXReg(insn.Rs1()) + insn.ImmI()) 
        & ~Register{1};

    sim->SetXReg(insn.Rd(), hold_pc);
}

void CallbackBEQ(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return a == b; });
}

void CallbackBNE(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return a != b; });
}

void CallbackBLT(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return RegToIReg(a) < RegToIReg(b); });
}

void CallbackBGE(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return RegToIReg(a) >= RegToIReg(b); });
}

void CallbackBLTU(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return a < b; });
}

void CallbackBGEU(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteBranch(sim, raw_insn, [](Register a, Register b) { return a >= b; });
}

void CallbackLB(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().ReadU8(addr);

    sim->SetXReg(
        insn.Rd(),
        SignExtT<uint32_t>(mem_val)
    );

    sim->Ip() += kStepSize;
}

void CallbackLH(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().ReadU16(addr);

    sim->SetXReg(
        insn.Rd(),
        SignExtT<uint32_t>(mem_val)
    );

    sim->Ip() += kStepSize;
}

void CallbackLW(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().ReadU32(addr);

    sim->SetXReg(
        insn.Rd(),
        mem_val
    );

    sim->Ip() += kStepSize;
}

void CallbackLBU(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().ReadU8(addr);

    sim->SetXReg(
        insn.Rd(),
        mem_val
    );
    
    sim->Ip() += kStepSize;
}

void CallbackLHU(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    auto mem_val = sim->Mem().ReadU16(addr);

    sim->SetXReg(
        insn.Rd(),
        mem_val
    );

    sim->Ip() += kStepSize;
}

void CallbackSB(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
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

void CallbackSH(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
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

void CallbackSW(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmS();
    auto val = sim->GetXReg(insn.Rs2());

    sim->Mem().StoreU32(addr, val);

    sim->Ip() += kStepSize;
}

void CallbackADDI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRIBinOp(sim, raw_insn, [](Register a, Register b) { return a + b; });
}

void CallbackSLTI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
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

void CallbackSLTIU(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
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

void CallbackXORI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRIBinOp(sim, raw_insn, [](Register a, Register b) { return a ^ b; });
}

void CallbackORI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRIBinOp(sim, raw_insn, [](Register a, Register b) { return a | b; });
}

void CallbackANDI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRIBinOp(sim, raw_insn, [](Register a, Register b) { return a & b; });
}

void CallbackSLLI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRIShift(sim, raw_insn, [](Register a, Register b) { return a << b; });
}

void CallbackSRLI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRIShift(sim, raw_insn, [](Register a, Register b) { return a >> b; });
}

void CallbackSRAI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRIShift(
        sim, 
        raw_insn, 
        [](Register a, Register b) { return ArithmRightShift(a, b); }
    );
}

void CallbackADD(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a + b; });
}

void CallbackSUB(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a - b; });
}

void CallbackSLL(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a << b; });
}

void CallbackSLT(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
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

void CallbackSLTU(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
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

void CallbackXOR(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a ^ b; });
}

void CallbackSRL(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a >> b; });
}

void CallbackSRA(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    return ExecuteRRRBinOp(
        sim, 
        raw_insn, 
        [](Register a, Register b) { return ArithmRightShift(a, b); }
    );
}

void CallbackOR(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a | b; });
}

void CallbackAND(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
   
    return ExecuteRRRBinOp(sim, raw_insn, [](Register a, Register b) { return a & b; });
}

void CallbackFENCE(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    assert(!"instruction not implemented");
}

void CallbackFENCE_I(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    assert(!"instruction not implemented");
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

    sim->ShouldStop() = true;

    sim->Ip() += kStepSize;
}

} // namespace isa
