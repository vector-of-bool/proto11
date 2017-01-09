#ifndef PROTO11_ENCODER_VARINT_HPP_INCLUDED
#define PROTO11_ENCODER_VARINT_HPP_INCLUDED

#include <cstdint>
#include <utility>

namespace proto11 {

using byte = std::uint8_t;

constexpr byte MSB_MASK = 1 << 7;

template <typename Integer, typename Iter> Iter write_varint(Integer value, Iter out) {
    // Varints are fairly simple to encode.
    // Just peel off and encode seven bits until the value is zero
    static_assert(!std::is_signed<Integer>::value, "Attempting to write signed number as a varint.");
    byte tmp[12] = { 0 };
    byte* tmp_out = tmp;
    do {
        *tmp_out++ = (value & Integer(0xff)) & ~MSB_MASK | MSB_MASK;
        value = value >> 7;
    } while (value != 0);
    // Unset the msb on the final encoded byte
    tmp_out[-1] ^= MSB_MASK;
    return std::copy(static_cast<byte*>(tmp), tmp_out, out);
}

template <typename Iter> std::pair<std::uint64_t, Iter> read_varint(Iter first, const Iter last) {
    byte tmp[12] = { 0 };
    byte* tmp_iter = tmp;
    while (first != last && (*first & MSB_MASK)) {
        *tmp_iter++ = *first++;
    }
    if (first == last) {
        // Got to end of stream with trailing varint (expected byte with MSB unset)
        throw std::runtime_error("Fail");
    }
    // Read final part of varint
    *tmp_iter++ = *first++;
    // Decode by catting the 7 bit sequence (in reverse, of course)
    std::uint64_t ret = 0;
    while (tmp_iter-- != tmp) {
        // Make room for next sevent bits
        ret <<= 7;
        // Stuff those bits in there.
        ret |= (*tmp_iter & ~MSB_MASK);
    }
    return { ret, first };
}

constexpr std::uint64_t zigzag_encode(std::int64_t value) {
    return (value << 1) ^ (value >> 63);
}

constexpr std::int64_t zigzag_decode(std::uint64_t value) {
    return (value >> 1) ^ (-(static_cast<std::int64_t>(value & 1)));
}

}  // namespace proto11

#endif  // PROTO11_ENCODER_VARINT_HPP_INCLUDED