#ifndef PROTO11_ENCODER_MESSAGE_HPP_INCLUDED
#define PROTO11_ENCODER_MESSAGE_HPP_INCLUDED

#include "const.hpp"

#include "varint.hpp"

#include <iterator>

namespace proto11 {

class message {};

namespace detail {

struct field_header {
    std::uint64_t field;
    wire_type type;
};

template <typename T, typename Iter>
std::pair<T, Iter> read_length_delim(Iter iter, const Iter last, std::input_iterator_tag) {
    std::uint64_t length = 0;
    std::tie(length, iter) = read_varint(iter, last);
    std::string buffer;
    std::copy_n(iter, length, std::back_inserter(buffer));
    return { proto11::read<T>(buffer.begin(), buffer.end()).first, iter };
}

template <typename T, typename Iter> std::pair<T, Iter> read_integral(field_header h, Iter first, Iter last) {
    switch (h.type) {
    case wire_type::varint: {
        const auto pair = read_varint(first, last);
        return { static_cast<T>(pair.first), pair.second };
    }
    case wire_type::fixed64:
    case wire_type::fixed32:
    // TOOD
    case wire_type::length_delimited:
    default:
        std::terminate();
    }
    // return read_field(h, first, last, );
}

template <typename Iter> std::pair<field_header, Iter> read_field_header(Iter first, const Iter last) {
    auto pair = read_varint(first, last);
    first = pair.second;
    const auto header_int = pair.first;
    field_header ret;
    ret.type = static_cast<wire_type>(header_int & 0b111);
    ret.field = header_int >> 3;
    return { ret, first };
}

template <typename Iter> Iter write_field_header(field_header h, Iter out) {
    return write_varint(static_cast<std::uint64_t>(h.field << 3 | static_cast<int>(h.type)), out);
}
}

template <typename Iter> message decode_message(Iter first, const Iter last) {}

}  // namespace proto11

#endif  // PROTO11_ENCODER_MESSAGE_HPP_INCLUDED