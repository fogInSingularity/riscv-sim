#include "isa/ext_zicsr_cb.hpp"

#include <cstddef>
#include <cassert>
#include <cstdint>
#include <limits>

#include "helpers/trace_calls.hpp"
#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"
#include "isa/rv_insn.hpp"
#include "isa/isa_hlp.hpp"

namespace isa {

void CallbackCSRRW(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto csr_idx = insn.ImmI();
    auto tmp = sim->GetCSR(csr_idx);
    sim->SetCSR(
        csr_idx, 
        sim->GetXReg(insn.Rs1())    
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Ip() += kStepSize;
}

void CallbackCSRRS(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto csr_idx = insn.ImmI();
    auto tmp = sim->GetCSR(csr_idx);
    sim->SetCSR(
        csr_idx, 
        tmp | sim->GetXReg(insn.Rs1())    
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Ip() += kStepSize;
}

void CallbackCSRRC(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto csr_idx = insn.ImmI();
    auto tmp = sim->GetCSR(csr_idx);
    sim->SetCSR(
        csr_idx, 
        tmp & ~sim->GetXReg(insn.Rs1())    
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Ip() += kStepSize;
}

// uimm encoded the same as rs1
void CallbackCSRRWI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto csr_idx = insn.ImmI();
    auto tmp = sim->GetCSR(csr_idx);
    auto uimm = insn.Rs1();    
    sim->SetCSR(
        csr_idx, 
        uimm
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Ip() += kStepSize;
}

void CallbackCSRRSI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto csr_idx = insn.ImmI();
    auto tmp = sim->GetCSR(csr_idx);
    auto uimm = insn.Rs1();
    sim->SetCSR(
        csr_idx, 
        tmp | uimm
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Ip() += kStepSize;
}

void CallbackCSRRCI(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto csr_idx = insn.ImmI();
    auto tmp = sim->GetCSR(csr_idx);
    auto uimm = insn.Rs1();
    sim->SetCSR(
        csr_idx, 
        tmp & ~uimm
    );

    sim->SetXReg(insn.Rd(), tmp);

    sim->Ip() += kStepSize;
}

} // namespace isa
