#ifndef ISA_HLP_HPP_
#define ISA_HLP_HPP_

#include <climits>
#include <bit>
#include <limits>
#include <type_traits>

#include "isa/isa_defs.hpp"

namespace isa {

inline IRegister RegToIReg(Register val) {
    return std::bit_cast<IRegister>(val);
}

inline Register IRegToReg(IRegister val) {
    return std::bit_cast<Register>(val);
}

inline WIRegister WRegToWIReg(WRegister val) {
    return std::bit_cast<WIRegister>(val);
}

inline WRegister WIRegToWReg(WIRegister val) {
    return std::bit_cast<WRegister>(val);
}

// FIXME
template <typename ToT, typename FromT>
constexpr ToT TruncHigh(FromT x) {
    static_assert(std::is_integral_v<FromT>, "FromT must be an integral type");
    static_assert(std::is_integral_v<ToT>, "ToT must be an integral type");
    static_assert(
        sizeof(ToT) < sizeof(FromT),
        "ToT must be strictly smaller than FromT"
    );

    using UnsignedFrom = std::make_unsigned_t<FromT>;
    using UnsignedTo = std::make_unsigned_t<ToT>;

    constexpr UnsignedTo to_max = std::numeric_limits<UnsignedTo>::max();
    constexpr UnsignedFrom mask = static_cast<UnsignedFrom>(to_max);

    const UnsignedFrom ux = static_cast<UnsignedFrom>(x);
    const UnsignedTo uy = static_cast<UnsignedTo>(ux & mask);

    return static_cast<ToT>(uy);
}

template <typename ToT, typename FromT>
constexpr ToT TruncLow(FromT x) {
    static_assert(std::is_integral_v<FromT>, "FromT must be an integral type");
    static_assert(std::is_integral_v<ToT>, "ToT must be an integral type");
    static_assert(
        sizeof(ToT) < sizeof(FromT),
        "ToT must be strictly smaller than FromT"
    );

    using UnsignedFrom = std::make_unsigned_t<FromT>;
    using UnsignedTo   = std::make_unsigned_t<ToT>;

    constexpr unsigned from_bits = std::numeric_limits<UnsignedFrom>::digits;
    constexpr unsigned to_bits = std::numeric_limits<UnsignedTo>::digits;
    static_assert(to_bits < from_bits, "Bit-width of ToT must be smaller");

    constexpr UnsignedTo to_max = std::numeric_limits<UnsignedTo>::max();
    constexpr UnsignedFrom mask = static_cast<UnsignedFrom>(to_max);

    const UnsignedFrom ux = static_cast<UnsignedFrom>(x);

    const UnsignedFrom shifted = ux >> (from_bits - to_bits);

    const UnsignedTo uy = static_cast<UnsignedTo>(shifted & mask);

    return static_cast<ToT>(uy);
}

template <typename ToT, typename FromT>
constexpr ToT SignExt(FromT x) {
    static_assert(std::is_unsigned_v<FromT>, "FromT must be unsigned");
    static_assert(std::is_unsigned_v<ToT>, "ToT must be unsigned");

    constexpr unsigned from_bits = sizeof(FromT) * CHAR_BIT;
    constexpr unsigned to_bits = sizeof(ToT) * CHAR_BIT;

    static_assert(
        from_bits < to_bits,
        "SignExtT expects ToT to be strictly wider than FromT"
    );

    constexpr ToT one = ToT{1};
    constexpr ToT value_mask = (one << from_bits) - one;
    constexpr ToT sign_bit = one << (from_bits - 1);
    constexpr ToT extend_mask = ~value_mask;

    ToT v = static_cast<ToT>(x) & value_mask;

    if (v & sign_bit) {
        v |= extend_mask;  // fill high bits
    }

    return v;
}

} // namespace isa

#endif // ISA_HLP_HPP_
