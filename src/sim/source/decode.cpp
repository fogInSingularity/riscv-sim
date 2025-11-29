#include "sim/decode.hpp"

#include <array>
#include <cstddef>

#include "isa/mnemonics.hpp"
#include "isa/isa_defs.hpp"

namespace sim {

using InsnMask = isa::UndecodedInsn;
using InsnMatch = isa::UndecodedInsn;

struct InsnEntry {
    isa::InsnMnemonic mnem;
    InsnMask mask;
    InsnMatch match;
};

constexpr std::array<InsnEntry, static_cast<size_t>(isa::InsnMnemonic::kCount)> InsnDecodeTable {{
#define MNEMONIC(name, mask, match) {isa::InsnMnemonic::name, mask, match},
#include "isa/isa_ext.inc"
#undef MNEMONIC
}};

isa::InsnMnemonic Decode(isa::UndecodedInsn insn) {
    for (auto& entry: InsnDecodeTable) {
        if ((insn & entry.mask) == entry.match) {
            return entry.mnem;
        }
    }

    return isa::InsnMnemonic::kInvalid;
}

} // namespace sim
