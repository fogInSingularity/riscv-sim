#ifndef DECODE_HPP_
#define DECODE_HPP_

#include "isa/mnemonics.hpp"
#include "sim/sim_defs.hpp"

namespace sim {
    
isa::InsnMnemonic Decode(sim::UndecodedInsn insn);

}

#endif // DECODE_HPP_
