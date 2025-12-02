#ifndef EXT_I_CB_HPP_
#define EXT_I_CB_HPP_

#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"

namespace isa {

#define MNEMONIC(name, mask, match) void Callback##name(sim::RVSim* sim, isa::UndecodedInsn raw_insn);
#include "isa/ext_i.inc"
#undef MNEMONIC

};

#endif // EXT_I_CB_HPP_
