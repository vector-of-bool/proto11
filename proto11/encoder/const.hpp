#ifndef PROTO11_ENCODER_CONST_HPP_INCLUDED
#define PROTO11_ENCODER_CONST_HPP_INCLUDED

namespace proto11 {

enum class wire_type {
    varint = 0,
    fixed64 = 1,
    length_delimited = 2,
    group_start = 3,
    group_end = 4,
    fixed32 = 5,
};

} // namespace proto11

#endif // PROTO11_ENCODER_CONST_HPP_INCLUDED