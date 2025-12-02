#ifndef RV_CSR_INSN_HPP_
#define RV_CSR_INSN_HPP_

#include <cstddef>
#include <cstdint>

#include "isa/isa_defs.hpp"
#include "isa/rv_insn.hpp"

namespace isa {

class RvCSRInsn : public RvInsn {
  public:
    explicit RvCSRInsn(isa::UndecodedInsn raw_insn) : RvInsn{raw_insn} {}
    uint32_t ImmCSR() const {
        constexpr uint32_t kImmShift = 20;
        // constexpr unsigned kImmBits  = 12;
        uint32_t imm = raw_insn_ >> kImmShift;
        return imm;
    }
};

} // namespace isa

#endif // RV_ CSR_INSN_HPP_
