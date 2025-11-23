#include "sim/dispatch.hpp"

#include <cstddef>
#include <array>

#include "isa/mnemonics.hpp"
#include "isa/ext_i_cb.hpp"

namespace sim {

namespace {

std::array<InsnCallback, static_cast<size_t>(isa::InsnMnemonic::kCount)> callback_table {
#define MNEMONIC(name, mask, match) isa::Callback##name,
#include "isa/ext_i.inc"
#undef MNEMONIC
};

} // namespace

InsnCallback GetInsnCallback(isa::InsnMnemonic mnem) {
    return callback_table[static_cast<size_t>(mnem)];
}

} // namespace sim
