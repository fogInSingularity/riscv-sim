#ifndef EXT_M_CB_HPP_
#define EXT_M_CB_HPP_

#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"

namespace isa {

#define MNEMONIC(name, mask, match) void Callback##name(sim::RVSim* sim, isa::UndecodedInsn raw_insn);
#include "isa/ext_m.inc"
#undef MNEMONIC

};

#endif // EXT_M_CB_HPP_
