#ifndef RV_F_INSN_HPP_
#define RV_F_INSN_HPP_

#include <cstddef>
#include <cstdint>

#include <spdlog/spdlog.h>

#include "isa/isa_defs.hpp"

namespace isa {

class RvFInsn {
  private:
    isa::UndecodedInsn raw_insn_;

    static constexpr int32_t SignExtend(uint32_t value, unsigned bits) {
        const uint32_t sign_bit = 1u << (bits - 1);
        const uint32_t masked   = value & ((1u << bits) - 1);
        const uint32_t extended = (masked ^ sign_bit) - sign_bit;
        return static_cast<int32_t>(extended);
    }

  public:
    explicit RvFInsn(isa::UndecodedInsn raw_insn) : raw_insn_{raw_insn} {}

    isa::UndecodedInsn Raw() const {
        return raw_insn_;
    }

    uint32_t Opcode() const {
        constexpr uint32_t kOpcodeMask = 0x7Fu;
        return raw_insn_ & kOpcodeMask;
    }

    // FP destination register (rd field interpreted as f-register index)
    uint32_t Rd() const {
        constexpr uint32_t kFrdShift = 7;
        constexpr uint32_t kFrdMask  = 0x1Fu;
        auto frd = (raw_insn_ >> kFrdShift) & kFrdMask;
        spdlog::trace("Frd(): frd = {}", frd);
        return frd;
    }

    // FP source register 1 (rs1 field interpreted as f-register index for OP-FP)
    uint32_t Rs1() const {
        constexpr uint32_t kFrs1Shift = 15;
        constexpr uint32_t kFrs1Mask  = 0x1Fu;
        auto frs1 = (raw_insn_ >> kFrs1Shift) & kFrs1Mask;
        spdlog::trace("Frs1(): frs1 = {}", frs1);
        return frs1;
    }

    // FP source register 2 (rs2 field interpreted as f-register index for OP-FP)
    uint32_t Rs2() const {
        constexpr uint32_t kFrs2Shift = 20;
        constexpr uint32_t kFrs2Mask  = 0x1Fu;
        return (raw_insn_ >> kFrs2Shift) & kFrs2Mask;
    }

    // FP source register 3 (rs3 field, used by fused multiply-add R4-type instructions)
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

    // FP format field (fmt, bits 26:25; 0 = S, 1 = D, etc.)
    uint32_t Fmt() const {
        constexpr uint32_t kFmtShift = 25;
        constexpr uint32_t kFmtMask  = 0x3u;
        return (raw_insn_ >> kFmtShift) & kFmtMask;
    }

    // For OP-FP (non-R4) this is funct5 in bits 31:27
    uint32_t Funct5() const {
        constexpr uint32_t kFunct5Shift = 27;
        constexpr uint32_t kFunct5Mask  = 0x1Fu;
        return (raw_insn_ >> kFunct5Shift) & kFunct5Mask;
    }

    // Full funct7 view (bits 31:25) if you want to treat it like a normal R-type
    uint32_t Funct7() const {
        constexpr uint32_t kFunct7Shift = 25;
        constexpr uint32_t kFunct7Mask  = 0x7Fu;
        return (raw_insn_ >> kFunct7Shift) & kFunct7Mask;
    }

    // I-type immediate (for FLW and integer→FP moves that use I-type layout)
    int32_t ImmI() const {
        constexpr uint32_t kImmShift = 20;
        constexpr unsigned kImmBits  = 12;
        uint32_t imm = raw_insn_ >> kImmShift;
        auto sext = SignExtend(imm, kImmBits);
        spdlog::trace("F ImmI(): {}", sext);
        return sext;
    }

    // S-type immediate (for FSW)
    int32_t ImmS() const {
        constexpr uint32_t kLowShift    = 7;
        constexpr uint32_t kLowMask     = 0x1Fu;
        constexpr uint32_t kHighShift   = 25;
        constexpr uint32_t kHighMask    = 0x7Fu;
        constexpr unsigned kImmBits     = 12;

        uint32_t low  = (raw_insn_ >> kLowShift)  & kLowMask;
        uint32_t high = (raw_insn_ >> kHighShift) & kHighMask;
        uint32_t imm  = low | (high << 5);
        return SignExtend(imm, kImmBits);
    }
};

} // namespace isa

#endif // RV_F_INSN_HPP_
