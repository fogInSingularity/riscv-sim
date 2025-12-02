#include "isa/ext_m_cb.hpp"

#include <cassert>
#include <limits>

#include "helpers/trace_calls.hpp"
#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"
#include "isa/rv_insn.hpp"
#include "isa/isa_hlp.hpp"

namespace isa {

void CallbackMUL(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    WRegister rs1 = sim->GetXReg(insn.Rs1());
    WRegister rs2 = sim->GetXReg(insn.Rs2());
    auto mul = rs1 * rs2;

    auto mul_trunc_low = TruncHigh<Register>(mul);

    sim->SetXReg(insn.Rd(), mul_trunc_low);

    sim->Step();
}

void CallbackMULH(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    WIRegister rs1 = sim->GetXReg(insn.Rs1());
    WIRegister rs2 = sim->GetXReg(insn.Rs2());

    auto mul = rs1 * rs2;

    auto mul_trunc_high = TruncLow<Register>(mul);

    sim->SetXReg(insn.Rd(), mul_trunc_high);

    sim->Step();
}

void CallbackMULHSU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    WIRegister rs1 = WRegToWIReg(
        SignExt<WRegister>(
            sim->GetXReg(insn.Rs1())
        )
    );
    WIRegister rs2 = RegToIReg(
        sim->GetXReg(insn.Rs2())
    );

    auto mul = rs1 * rs2;

    auto mul_trunc_high = TruncLow<Register>(mul);

    sim->SetXReg(insn.Rd(), mul_trunc_high);

    sim->Step();
}

void CallbackMULHU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    WRegister rs1 = sim->GetXReg(insn.Rs1());
    WRegister rs2 = sim->GetXReg(insn.Rs2());

    auto mul = rs1 * rs2;

    auto mul_trunc_high = TruncLow<Register>(mul);

    sim->SetXReg(insn.Rd(), mul_trunc_high);

    sim->Step();
}

void CallbackDIV(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    IRegister rs1 = RegToIReg(sim->GetXReg(insn.Rs1()));
    IRegister rs2 = RegToIReg(sim->GetXReg(insn.Rs2()));

    IRegister res = 0;
    if (rs2 == 0) {
        res = -1;
    } else if (rs1 == std::numeric_limits<IRegister>::min() && rs2 == -1) {
        res = std::numeric_limits<IRegister>::min();
    } else {
        res = rs1 / rs2;
    }

    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackDIVU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    Register rs1 = sim->GetXReg(insn.Rs1());
    Register rs2 = sim->GetXReg(insn.Rs2());

    Register res = 0;
    if (rs2 == 0) {
        res = std::numeric_limits<Register>::max();
    } else {
        res = rs1 / rs2;
    }

    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackREM(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    IRegister rs1 = RegToIReg(sim->GetXReg(insn.Rs1()));
    IRegister rs2 = RegToIReg(sim->GetXReg(insn.Rs2()));

    Register res = 0;
    if (rs2 == 0) {
        res = rs1;
    } else if (rs1 == std::numeric_limits<IRegister>::min() && rs2 == -1) {
        res = 0;
    } else {
        res = rs1 % rs2;
    }

    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackREMU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    Register rs1 = sim->GetXReg(insn.Rs1());
    Register rs2 = sim->GetXReg(insn.Rs2());

    Register res = 0;

    if (rs2 == 0) {
        res = rs1;
    } else {
        res = rs1 % rs2;
    }

    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

} // namespace isa
