#ifndef MNEMONICS_HPP_
#define MNEMONICS_HPP_

namespace isa {

enum class InsnMnemonic {

#define MNEMONIC(name, mask, match) name,
#include "isa/ext_i.inc"
#undef MNEMONIC
    
    kCount,
    kInvalid,
};

const char* MnemonicToStr(InsnMnemonic mnem);

} // namespace isa

#endif // MNEMONICS_HPP_
