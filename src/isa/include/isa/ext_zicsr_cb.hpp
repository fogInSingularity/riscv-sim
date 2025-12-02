#ifndef EXT_ZICSR_CB_HPP_
#define EXT_ZICSR_CB_HPP_

#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"

namespace isa {

#define MNEMONIC(name, mask, match) void Callback##name(sim::RVSim* sim, isa::UndecodedInsn raw_insn);
#include "isa/ext_zicsr.inc"
#undef MNEMONIC

};

#endif // EXT_ZICSR_CB_HPP_
