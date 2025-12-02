#ifndef RV_F_INSN_HPP_
#define RV_F_INSN_HPP_

#include <cstddef>
#include <cstdint>

#include <spdlog/spdlog.h>

#include "isa/isa_defs.hpp"
#include "isa/rv_insn.hpp"

namespace isa {

class RvFInsn : public RvInsn {
  public:
    explicit RvFInsn(isa::UndecodedInsn raw_insn) : RvInsn{raw_insn} {}

    // for rd <- r1, r2, r3 instructions (like fma)
    uint32_t Rs3() const {
        constexpr uint32_t kFrs3Shift = 27;
        constexpr uint32_t kFrs3Mask  = 0x1Fu;
        return (raw_insn_ >> kFrs3Shift) & kFrs3Mask;
    }

    // Rounding mode field (rm, bits 14:12)
    uint32_t Rm() const {
        constexpr uint32_t kRmShift = 12;
        constexpr uint32_t kRmMask  = 0x7u;
        return (raw_insn_ >> kRmShift) & kRmMask;
    }

    // format field (fmt, bits 26:25; 0 = S, 1 = D, etc.)
    uint32_t Fmt() const {
        constexpr uint32_t kFmtShift = 25;
        constexpr uint32_t kFmtMask  = 0x3u;
        return (raw_insn_ >> kFmtShift) & kFmtMask;
    }

    // OP-FP (non-R4) (funct5, in bits 31:27)
    uint32_t Funct5() const {
        constexpr uint32_t kFunct5Shift = 27;
        constexpr uint32_t kFunct5Mask  = 0x1Fu;
        return (raw_insn_ >> kFunct5Shift) & kFunct5Mask;
    }
};

} // namespace isa

#endif // RV_F_INSN_HPP_
