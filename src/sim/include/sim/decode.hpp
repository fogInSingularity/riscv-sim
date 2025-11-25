#ifndef DECODE_HPP_
#define DECODE_HPP_

#include "isa/mnemonics.hpp"
#include "isa/isa_defs.hpp"

namespace sim {
    
isa::InsnMnemonic Decode(isa::UndecodedInsn insn);

}

#endif // DECODE_HPP_
