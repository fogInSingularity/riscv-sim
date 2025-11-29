#include "isa/mnemonics.hpp"

#include "helpers/common.hpp"

namespace isa {

const char* MnemonicToStr(InsnMnemonic mnem) {
    switch (mnem) {
#define MNEMONIC(name, mask, match) case InsnMnemonic::name: return TO_STR(name);
#include "isa/isa_ext.inc"
#undef MNEMONIC
        default: return "<unknown>";
    }
}

} // namespace isa

