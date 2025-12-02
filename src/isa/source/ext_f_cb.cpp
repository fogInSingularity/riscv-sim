#include "isa/ext_f_cb.hpp"

#include <bit>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <limits>

#include "helpers/trace_calls.hpp"
#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"
#include "isa/rvf_insn.hpp"
#include "isa/isa_hlp.hpp"
#include "softfloat.h"
#include "softfloat_wrap.hpp"

namespace isa {

namespace {

auto SoftFloatSetRM(isa::RoundingMode rm) {
    switch (rm) {
        using enum isa::RoundingMode;
        case RNE:
            return softfloat_roundingMode = softfloat_round_near_even;
        case RTZ:
            return softfloat_roundingMode = softfloat_round_minMag;
        case RDN:
            return softfloat_roundingMode = softfloat_round_min;
        case RUP:
            return softfloat_roundingMode = softfloat_round_max;
        case RMM:
            return softfloat_roundingMode = softfloat_round_near_maxMag;
        case DYN:
            assert(!"what?");
            break;
    }

    return decltype(softfloat_roundingMode){0};
}

void SoftFloatClearFExcp() {
    softfloat_exceptionFlags = 0;
}

// FIXME: sim->UpdateExcp(softfloat_exceptionFlags)
void SoftFloatSetFExcp(sim::RVSim* sim) {
    assert(sim != nullptr);

    isa::Register excp_flags = softfloat_exceptionFlags & 0x1fu;
    if (excp_flags == 0) {
        return ; // no excp
    }

    auto sim_flags = sim->GetCSR(isa::CSR::fflags);
    sim->SetCSR(isa::CSR::fflags, sim_flags | excp_flags);
}

// FIXME nan correctness    
inline float32_t f32_min(float32_t a, float32_t b) {
    return f32_le(a, b) ? a : b;
}

inline float32_t f32_max(float32_t a, float32_t b) {
    return f32_le(a, b) ? b : a;
}

inline float32_t f32_neg(float32_t val) {
    auto sign_bit = decltype(val.v){1} << (sizeof(val.v) * CHAR_BIT - 1);
    val.v = ((val.v ^ sign_bit) & sign_bit) | (val.v & ~sign_bit);
    return val;
}

template <typename FBinOp>
void ExecuteRRRFBinOp(sim::RVSim* sim, isa::UndecodedInsn raw_insn, FBinOp fbin_op) {
    assert(sim != nullptr);
    hlp::trace_call();

    SoftFloatClearFExcp();

    RvFInsn insn{raw_insn};
    SoftFloatSetRM(sim->GetRM(insn.Rm()));

    auto res = fbin_op(
        sim->GetFReg(insn.Rs1()),
        sim->GetFReg(insn.Rs2())
    );
    sim->SetFReg(insn.Rd(), res);
    SoftFloatSetFExcp(sim);

    sim->Step();
}

template <typename FTernOp>
void ExecuteRRRRFTernOp(sim::RVSim* sim, isa::UndecodedInsn raw_insn, FTernOp ftern_op) {
    assert(sim != nullptr);
    hlp::trace_call();

    SoftFloatClearFExcp();

    RvFInsn insn{raw_insn};
    SoftFloatSetRM(sim->GetRM(insn.Rm()));

    auto res = ftern_op(
        sim->GetFReg(insn.Rs1()),
        sim->GetFReg(insn.Rs2()),
        sim->GetFReg(insn.Rs3())
    );
    sim->SetFReg(insn.Rd(), res);
    SoftFloatSetFExcp(sim);

    sim->Step();
}

template <typename FCmpOp>
void ExecuteRRRCmpOp(sim::RVSim* sim, isa::UndecodedInsn raw_insn, FCmpOp fcmp_op) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    SoftFloatClearFExcp();

    RvFInsn insn{raw_insn};

    auto cmp = fcmp_op(
        sim->GetFReg(insn.Rs1()),
        sim->GetFReg(insn.Rs2())
    );
    sim->SetXReg(insn.Rd(), cmp ? 1 : 0);
    SoftFloatSetFExcp(sim);

    sim->Step();
}

} // namespace

void CallbackFLW(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvFInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmI();
    sim->SetFReg(
        insn.Rd(), 
        RegToFReg(sim->Mem().LoadU32(addr))
    );

    sim->Step();
}

void CallbackFSW(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvFInsn insn{raw_insn};

    auto addr = sim->GetXReg(insn.Rs1()) + insn.ImmS();
    auto mem_val = FRegToReg(sim->GetFReg(insn.Rs2()));

    sim->Mem().StoreU32(addr, mem_val);

    sim->Step();
}

void CallbackFADD_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRFBinOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2) { return f32_add(rs1, rs2); }
    );
}

void CallbackFSUB_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRFBinOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2) { return f32_sub(rs1, rs2); }
    );
}

void CallbackFMUL_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRFBinOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2) { return f32_mul(rs1, rs2); }
    );
}

void CallbackFDIV_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRFBinOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2) { return f32_div(rs1, rs2); }
    );
}

void CallbackFSQRT_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    SoftFloatClearFExcp();

    RvFInsn insn{raw_insn};
    SoftFloatSetRM(sim->GetRM(insn.Rm()));

    auto res = f32_sqrt(
        sim->GetFReg(insn.Rs1())
    );
    sim->SetFReg(insn.Rd(), res);
    SoftFloatSetFExcp(sim);

    sim->Step();
}

void CallbackFMADD_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRRFTernOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2, isa::FRegister rs3) { 
            return f32_mulAdd(rs1, rs2, rs3);
        }
    );
}

void CallbackFMSUB_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRRFTernOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2, isa::FRegister rs3) { 
            return f32_mulAdd(rs1, rs2, f32_neg(rs3));
        }
    );
}

void CallbackFNMSUB_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRRFTernOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2, isa::FRegister rs3) { 
            return f32_mulAdd(f32_neg(rs1), rs2, rs3);
        }
    );
}

void CallbackFNMADD_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRRFTernOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2, isa::FRegister rs3) { 
            return f32_neg(f32_mulAdd(rs1, rs2, rs3));
        }
    );
}

void CallbackFSGNJ_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();
    
    RvFInsn insn{raw_insn}; 
    
    auto rs1 = sim->GetFReg(insn.Rs1());
    auto rs2 = sim->GetFReg(insn.Rs2());

    isa::FRegister res{};
    isa::Register high_bit = isa::Register{1} << (sizeof(isa::Register) * CHAR_BIT - 1);
    res.v = (rs2.v & high_bit) | (rs1.v & ~high_bit);
    
    sim->SetFReg(insn.Rd(), res);

    sim->Step();
}

void CallbackFSGNJN_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvFInsn insn{raw_insn};
    
    auto rs1 = sim->GetFReg(insn.Rs1());
    auto rs2 = sim->GetFReg(insn.Rs2());

    isa::FRegister res{};
    isa::Register high_bit = isa::Register{1} << (sizeof(isa::Register) * CHAR_BIT - 1);
    res.v = ((rs2.v ^ high_bit) & high_bit) | (rs1.v & ~high_bit);
    
    sim->SetFReg(insn.Rd(), res);

    sim->Step();
}

void CallbackFSGNJX_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvFInsn insn{raw_insn};
    
    auto rs1 = sim->GetFReg(insn.Rs1());
    auto rs2 = sim->GetFReg(insn.Rs2());

    isa::FRegister res{};
    isa::Register high_bit = isa::Register{1} << (sizeof(isa::Register) * CHAR_BIT - 1);
    res.v = ((rs1.v & high_bit) ^ (rs2.v & high_bit)) | (rs1.v & ~high_bit);
 
    sim->SetFReg(insn.Rd(), res);

    sim->Step();
}

void CallbackFMIN_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvFInsn insn{raw_insn};

    auto res = f32_min(
        sim->GetFReg(insn.Rs1()),
        sim->GetFReg(insn.Rs2())
    );
    sim->SetFReg(insn.Rd(), res);
        
    sim->Step();
}

void CallbackFMAX_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvFInsn insn{raw_insn};

    auto res = f32_max(
        sim->GetFReg(insn.Rs1()),
        sim->GetFReg(insn.Rs2())
    );
    sim->SetFReg(insn.Rd(), res);

    sim->Step();
}

void CallbackFLE_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRCmpOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2) { return f32_le(rs1, rs2); }
    );
}

void CallbackFLT_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRCmpOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2) { return f32_lt(rs1, rs2); }
    );
}

void CallbackFEQ_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    return ExecuteRRRCmpOp(
        sim, 
        raw_insn, 
        [](isa::FRegister rs1, isa::FRegister rs2) { return f32_eq(rs1, rs2); }
    );
}

void CallbackFCVT_W_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    SoftFloatClearFExcp();

    RvFInsn insn{raw_insn};

    auto frs1 = sim->GetFReg(insn.Rs1());
    bool raise_inexact = true;
    auto conv = IRegToReg(f32_to_i32(
        frs1, 
        SoftFloatSetRM(sim->GetRM(insn.Rm())), 
        raise_inexact
    ));

    sim->SetXReg(insn.Rd(), conv);
    SoftFloatSetFExcp(sim);

    sim->Step();
}

void CallbackFCVT_WU_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    SoftFloatClearFExcp();

    RvFInsn insn{raw_insn};

    auto frs1 = sim->GetFReg(insn.Rs1());
    bool raise_inexact = true;
    auto conv = f32_to_ui32(
        frs1, 
        SoftFloatSetRM(sim->GetRM(insn.Rm())), 
        raise_inexact
    );

    sim->SetXReg(insn.Rd(), conv);
    SoftFloatSetFExcp(sim);

    sim->Step();
}

void CallbackFCVT_S_W(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    SoftFloatClearFExcp();

    RvFInsn insn{raw_insn};

    auto rs1 = RegToIReg(sim->GetXReg(insn.Rs1()));
    auto conv = i32_to_f32(rs1);
    sim->SetFReg(insn.Rd(), conv); 
    SoftFloatSetFExcp(sim);

    sim->Step();
}

void CallbackFCVT_S_WU(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    SoftFloatClearFExcp();
        
    RvFInsn insn{raw_insn};

    auto rs1 = sim->GetXReg(insn.Rs1());
    auto conv = ui32_to_f32(rs1);
    sim->SetFReg(insn.Rd(), conv);

    SoftFloatSetFExcp(sim);

    sim->Step();
}

void CallbackFMV_X_W(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvFInsn insn{raw_insn};

    auto frs1 = sim->GetFReg(insn.Rs1());
    sim->SetXReg(insn.Rd(), FRegToReg(frs1));

    sim->Step();
}

void CallbackFMV_W_X(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvFInsn insn{raw_insn};

    auto rs1 = sim->GetXReg(insn.Rs1());
    sim->SetFReg(insn.Rd(), RegToFReg(rs1));

    sim->Step();
}

void CallbackFCLASS_S(sim::RVSim* sim, isa::UndecodedInsn raw_insn) {
    assert(sim != nullptr);
    hlp::trace_call();

    RvFInsn insn{raw_insn};
    
    auto rs = sim->GetFReg(insn.Rs1());
    static_assert(sizeof(rs) == sizeof(float));
    // hope float have same representation as float32_t, god bless
    auto rs_as_f = std::bit_cast<float>(rs.v);     
    auto hot_bit = isa::Register{1};

    auto classify = std::fpclassify(rs_as_f);
    auto shift = 0;
    switch (classify) {
        case FP_INFINITE:
            shift = std::signbit(rs_as_f) ? 0 : 7;
            break;
        case FP_NAN:
            shift = f32_isSignalingNaN(rs) ? 8 : 9;
            break;
        case FP_NORMAL:
            shift = std::signbit(rs_as_f) ? 1 : 6;
            break;
        case FP_SUBNORMAL:
            shift = std::signbit(rs_as_f) ? 2 : 5;
            break;
        case FP_ZERO:
            shift = std::signbit(rs_as_f) ? 3 : 4;
            break;
        default:
            assert(!"unknown classify value");
            break;
    }

    hot_bit <<= shift;

    sim->SetXReg(insn.Rd(), hot_bit);

    sim->Step();
}

} // namespace isa
