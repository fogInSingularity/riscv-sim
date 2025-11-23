#include "sim/decode.hpp"

#include <array>
#include <cstddef>

#include "isa/mnemonics.hpp"
#include "sim/sim_defs.hpp"

namespace sim {

using InsnMask = sim::UndecodedInsn;
using InsnMatch = sim::UndecodedInsn;

struct InsnEntry {
    isa::InsnMnemonic mnem;
    InsnMask mask;
    InsnMatch match;
};

constexpr std::array<InsnEntry, static_cast<size_t>(isa::InsnMnemonic::kCount)> InsnDecodeTable {{
#define MNEMONIC(name, mask, match) {isa::InsnMnemonic::name, mask, match},
#include "isa/ext_i.inc"
#undef MNEMONIC
}};

isa::InsnMnemonic Decode(sim::UndecodedInsn insn) {
    for (auto& entry: InsnDecodeTable) {
        if ((insn & entry.mask) == entry.match) {
            return entry.mnem;
        }
    }

    return isa::InsnMnemonic::kInvalid;
}

} // namespace sim
