#ifndef RV_INSN_HPP_
#define RV_INSN_HPP_

#include <cstddef>
#include <cstdint>

#include <spdlog/spdlog.h>

#include "isa/isa_defs.hpp"

namespace isa {

class RvInsn {
  protected:
    isa::UndecodedInsn raw_insn_;

    static constexpr int32_t SignExtend(uint32_t value, unsigned bits) {
        const uint32_t sign_bit = 1u << (bits - 1);
        const uint32_t masked   = value & ((1u << bits) - 1);
        const uint32_t extended = (masked ^ sign_bit) - sign_bit;
        return static_cast<int32_t>(extended);
    }

  public:
    explicit RvInsn(isa::UndecodedInsn raw_insn) : raw_insn_{raw_insn} {}

    isa::UndecodedInsn Raw() const {
        return raw_insn_;
    }

    uint32_t Opcode() const {
        constexpr uint32_t kOpcodeMask = 0x7Fu;
        return raw_insn_ & kOpcodeMask;
    }

    uint32_t Rd() const {
        constexpr uint32_t kRdShift = 7;
        constexpr uint32_t kRdMask  = 0x1Fu;
        auto rd = (raw_insn_ >> kRdShift) & kRdMask;
        SPDLOG_TRACE("Rd(): rd = {}", rd);
        return rd;
    }

    uint32_t Funct3() const {
        constexpr uint32_t kFunct3Shift = 12;
        constexpr uint32_t kFunct3Mask  = 0x7u;
        return (raw_insn_ >> kFunct3Shift) & kFunct3Mask;
    }

    uint32_t Rs1() const {
        constexpr uint32_t kRs1Shift = 15;
        constexpr uint32_t kRs1Mask  = 0x1Fu;
        auto rs1 = (raw_insn_ >> kRs1Shift) & kRs1Mask;
        SPDLOG_TRACE("Rs1(): rs1 = {}", rs1);
        return rs1;
    }

    uint32_t Rs2() const {
        constexpr uint32_t kRs2Shift = 20;
        constexpr uint32_t kRs2Mask  = 0x1Fu;
        return (raw_insn_ >> kRs2Shift) & kRs2Mask;
    }

    uint32_t Funct7() const {
        constexpr uint32_t kFunct7Shift = 25;
        constexpr uint32_t kFunct7Mask  = 0x7Fu;
        return (raw_insn_ >> kFunct7Shift) & kFunct7Mask;
    }

    uint32_t Shamt() const {
        constexpr uint32_t kShamtShift = 20;
        constexpr uint32_t kShamtMask  = 0x1Fu;
        return (raw_insn_ >> kShamtShift) & kShamtMask;
    }

    int32_t ImmI() const {
        constexpr uint32_t kImmShift = 20;
        constexpr unsigned kImmBits  = 12;
        uint32_t imm = raw_insn_ >> kImmShift;
        auto sext = SignExtend(imm, kImmBits);
        SPDLOG_TRACE("i immid: {}", sext);
        return sext;
    }

    int32_t ImmS() const {
        constexpr uint32_t kLowShift    = 7;
        constexpr uint32_t kLowMask     = 0x1Fu; // bits 4:0
        constexpr uint32_t kHighShift   = 25;
        constexpr uint32_t kHighMask    = 0x7Fu; // bits 11:5
        constexpr unsigned kImmBits     = 12;

        uint32_t low  = (raw_insn_ >> kLowShift)  & kLowMask;
        uint32_t high = (raw_insn_ >> kHighShift) & kHighMask;
        uint32_t imm  = low | (high << 5);
        return SignExtend(imm, kImmBits);
    }

    int32_t ImmB() const {
        // B-type immediate layout:
        // imm[12|10:5|4:1|11] from bits [31|30:25|11:8|7], then <<1
        constexpr unsigned kImmBits = 13;

        uint32_t bit11    = (raw_insn_ >> 7)  & 0x1u;
        uint32_t bits4_1  = (raw_insn_ >> 8)  & 0xFu;
        uint32_t bits10_5 = (raw_insn_ >> 25) & 0x3Fu;
        uint32_t bit12    = (raw_insn_ >> 31) & 0x1u;

        uint32_t imm =
            (bit12    << 12) |
            (bit11    << 11) |
            (bits10_5 << 5)  |
            (bits4_1  << 1);

        return SignExtend(imm, kImmBits);
    }

    int32_t ImmU() const {
        // U-type: imm[31:12] << 12
        constexpr uint32_t kImmMask = 0xFFFFF000u;
        uint32_t imm = raw_insn_ & kImmMask;
        return static_cast<int32_t>(imm);
    }

    int32_t ImmJ() const {
        // J-type immediate layout:
        // imm[20|10:1|11|19:12] from [31|30:21|20|19:12], then << 1
        constexpr unsigned kImmBits = 21;

        uint32_t bit20     = (raw_insn_ >> 31) & 0x1u;
        uint32_t bits10_1  = (raw_insn_ >> 21) & 0x3FFu;
        uint32_t bit11     = (raw_insn_ >> 20) & 0x1u;
        uint32_t bits19_12 = (raw_insn_ >> 12) & 0xFFu;

        uint32_t imm = (
            (bit20     << 20) 
            | (bits19_12 << 12) 
            | (bit11     << 11) 
            | (bits10_1  << 1)
        );

        auto sext = SignExtend(imm, kImmBits);
        SPDLOG_TRACE("j immid: {}", sext);
        return sext;
    }
};} // namespace isa


#endif // RV_INSN_HPP_
