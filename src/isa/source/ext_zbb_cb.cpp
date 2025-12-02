#include "isa/ext_zbb_cb.hpp"

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

void CallbackANDN(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto res = sim->GetXReg(insn.Rs1()) & ~sim->GetXReg(insn.Rs2());

    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackORN(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto res = sim->GetXReg(insn.Rs1()) | ~sim->GetXReg(insn.Rs2());

    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackXNOR(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto res = ~(sim->GetXReg(insn.Rs1()) ^ sim->GetXReg(insn.Rs2()));

    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackMIN(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto rs1 = sim->GetXReg(insn.Rs1());
    auto rs2 = sim->GetXReg(insn.Rs2());

    auto res = RegToIReg(rs1) < RegToIReg(rs2) ? rs1 : rs2;
    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackMINU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto rs1 = sim->GetXReg(insn.Rs1());
    auto rs2 = sim->GetXReg(insn.Rs2());

    auto res = rs1 < rs2 ? rs1 : rs2;
    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackMAX(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto rs1 = sim->GetXReg(insn.Rs1());
    auto rs2 = sim->GetXReg(insn.Rs2());

    auto res = RegToIReg(rs1) < RegToIReg(rs2) ? rs2 : rs1;
    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackMAXU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto rs1 = sim->GetXReg(insn.Rs1());
    auto rs2 = sim->GetXReg(insn.Rs2());

    auto res = rs1 < rs2 ? rs2 : rs1;
    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackSEXT_B(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto rs = sim->GetXReg(insn.Rs1());

    auto res = SignExt<Register>(TruncHigh<uint8_t>(rs));
    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackSEXT_H(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto rs = sim->GetXReg(insn.Rs1());

    auto res = SignExt<Register>(TruncHigh<uint16_t>(rs));
    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackZEXT_H(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto rs = sim->GetXReg(insn.Rs1());

    auto res = TruncHigh<uint16_t>(rs);
    sim->SetXReg(insn.Rd(), res);

    sim->Step();
}

void CallbackCLZ(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvInsn insn{raw_insn};

    auto rs = sim->GetXReg(insn.Rs1());

    int64_t highest_set = -1;

    for (int64_t i = kXlen - 1; i >= 0; i--) {
        if (IsBitSet(rs, i)) {
            highest_set = i;
            break;
        }
    }

    sim->SetXReg(
        insn.Rd(),
        (kXlen - 1) - highest_set
    );

    sim->Step();
}

void CallbackCTZ(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto rs = sim->GetXReg(insn.Rs1());

    size_t lowest_set = kXlen;

    for (size_t i = 0; i <= kXlen - 1; i++) {
        if (IsBitSet(rs, i)) {
            lowest_set = i;
            break;
        }
    }

    sim->SetXReg(
        insn.Rd(),
        lowest_set
    );

    sim->Step();
}

void CallbackCPOP(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto rs = sim->GetXReg(insn.Rs1());

    size_t bcount = 0;

    for (size_t i = 0; i <= kXlen - 1; i++) {
        if (IsBitSet(rs, i)) {
            bcount++;
        }
    }

    sim->SetXReg(
        insn.Rd(),
        bcount
    );

    sim->Step();
}

void CallbackROL(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto shamt = sim->GetXReg(insn.Rs2()) & (kXlen - 1);
    auto rs1 = sim->GetXReg(insn.Rs1());

    sim->SetXReg(
        insn.Rd(),
        (rs1 << shamt) | (rs1 >> (kXlen - shamt))
    );

    sim->Step();
}

void CallbackROR(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto shamt = sim->GetXReg(insn.Rs2()) & (kXlen - 1);
    auto rs1 = sim->GetXReg(insn.Rs1());

    sim->SetXReg(
        insn.Rd(),
        (rs1 >> shamt) | (rs1 << (kXlen - shamt))
    );

    sim->Step();
}

void CallbackRORI(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};

    auto shamt = insn.Shamt();
    auto rs1 = sim->GetXReg(insn.Rs1());

    sim->SetXReg(
        insn.Rd(),
        (rs1 >> shamt) | (rs1 << (kXlen - shamt))
    );

    sim->Step();
}

void CallbackREV8(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};
    auto rs = sim->GetXReg(insn.Rs1());
    Register val = 0;

    int64_t j = kXlenByte - 1;
    for (size_t i = 0; i < kXlenByte; i++, j--) {
        val = SetByte(val, GetByte(rs, j), i);
    }

    sim->SetXReg(
        insn.Rd(),
        val
    );

    sim->Step();
}

void CallbackORC_B(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvInsn insn{raw_insn};
    auto rs = sim->GetXReg(insn.Rs1());
    Register val = 0;

    for (size_t i = 0; i < kXlenByte; i++) {
        if (GetByte(rs, i) == 0) {
            val = SetByte(val, uint8_t{0}, i);
        } else {
            val = SetByte(val, ~uint8_t{0}, i);
        }
    }

    sim->SetXReg(
        insn.Rd(),
        val
    );

    sim->Step();
}

} // namespace isa
