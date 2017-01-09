#ifndef PROTO11_GENERATED_HPP_INCLUDED
#define PROTO11_GENERATED_HPP_INCLUDED

#include <string>
#include <type_traits>
#include <vector>


#include "encoder/encoder.hpp"
#include "encoder/message.hpp"

// This file contains utilities used by generated classes and types from the
// protoc compiler

namespace proto11 {

enum type_type {
    enum_,
    message,
};

template <typename Pointer, Pointer> struct field_traits;

#define PROTO11_FIELD_TRAITS(memptr) ::proto11::field_traits<decltype(memptr), memptr>

}  // namespace proto11

#endif  // PROTO11_GENERATED_HPP_INCLUDED