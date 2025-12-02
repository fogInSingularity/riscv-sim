#ifndef EXT_F_CB_HPP_
#define EXT_F_CB_HPP_

#include "sim/rv_sim.hpp"
#include "isa/isa_defs.hpp"

namespace isa {

#define MNEMONIC(name, mask, match) void Callback##name(sim::RVSim* sim, [[maybe_unused]] isa::UndecodedInsn raw_insn);
#include "isa/ext_f.inc"
#undef MNEMONIC

};

#endif // EXT_I_CB_HPP_
