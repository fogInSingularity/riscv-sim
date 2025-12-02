#include "isa/ext_zicsr_cb.hpp"

#include <cstddef>
#include <cassert>
#include <cstdint>
#include <limits>

#include "helpers/trace_calls.hpp"
#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"
#include "isa/rv_csr_insn.hpp"
#include "isa/isa_hlp.hpp"

namespace isa {

void CallbackCSRRW(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvCSRInsn insn{raw_insn};

    auto csr_idx = insn.ImmCSR();
    auto tmp = sim->GetCSR(csr_idx);
    sim->SetCSR(
        csr_idx, 
        sim->GetXReg(insn.Rs1())    
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Step();
}

void CallbackCSRRS(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvCSRInsn insn{raw_insn};

    auto csr_idx = insn.ImmCSR();
    auto tmp = sim->GetCSR(csr_idx);
    sim->SetCSR(
        csr_idx, 
        tmp | sim->GetXReg(insn.Rs1())    
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Step();
}

void CallbackCSRRC(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvCSRInsn insn{raw_insn};

    auto csr_idx = insn.ImmCSR();
    auto tmp = sim->GetCSR(csr_idx);
    sim->SetCSR(
        csr_idx, 
        tmp & ~sim->GetXReg(insn.Rs1())    
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Step();
}

// uimm encoded the same as rs1
void CallbackCSRRWI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvCSRInsn insn{raw_insn};

    auto csr_idx = insn.ImmCSR();
    auto tmp = sim->GetCSR(csr_idx);
    auto uimm = insn.Rs1();    
    sim->SetCSR(
        csr_idx, 
        uimm
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Step();
}

void CallbackCSRRSI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvCSRInsn insn{raw_insn};

    auto csr_idx = insn.ImmCSR();
    auto tmp = sim->GetCSR(csr_idx);
    auto uimm = insn.Rs1();
    sim->SetCSR(
        csr_idx, 
        tmp | uimm
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Step();
}

void CallbackCSRRCI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvCSRInsn insn{raw_insn};

    auto csr_idx = insn.ImmCSR();
    auto tmp = sim->GetCSR(csr_idx);
    auto uimm = insn.Rs1();
    sim->SetCSR(
        csr_idx, 
        tmp & ~uimm
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Step();
}

} // namespace isa
