#ifndef DISPATCH_HPP_
#define DISPATCH_HPP_

#include "isa/mnemonics.hpp"
#include "isa/isa_defs.hpp"
#include "sim/rv_sim.hpp"

namespace sim {

// For dispatch table to work correctly, name of function should be Callback<InsnName>

using InsnCallback = void (*) (RVSim* sim, isa::UndecodedInsn instr);

InsnCallback GetInsnCallback(isa::InsnMnemonic mnem);

} // namespace sim

#endif // DISPATCH_HPP_
