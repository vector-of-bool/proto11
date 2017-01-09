#ifndef PROTO11_CPP11_ENUM_HPP_INCLUDED
#define PROTO11_CPP11_ENUM_HPP_INCLUDED

#include <google/protobuf/Descriptor.h>

#include "helpers.hpp"

#include "emitter.hpp"

namespace proto11 {

void emit_enum(Emitter& em, const google::protobuf::EnumDescriptor& desc) {
    em.line("enum class ", desc.name(), " {");
    em.indented([&] {
        PROTO11_FOREACH(desc, value) { em.line(value.name(), " = ", value.number(), ","); };
    });
    em.line("};");

    em.tail([&] {
        em.line("namespace proto11 {");
        em.line();
        em.block("template <> struct traits<" + qualified_name(desc) + ", void> {", "};", [&] {
            em.block("enum {", "};", [&] {
                em.line("valid = true,");
                em.line("is_enum = true,");
                em.line("is_message = false,");
            });
            em.line("using check = std::true_type;");
            em.line("using type = ", qualified_name(desc), ';');
            em.block("template <typename Iter> static Iter write(type value, Iter iter) {", "}", [&] {
                em.line("return write_varint(static_cast<std::int64_t>(value), iter);");
            });
            em.block("template <typename Iter> static std::pair<type, Iter> read(Iter first, Iter last) {", "}", [&] {
                em.line("auto pair = read_varint(first, last);");
                em.line("return { static_cast<type>(pair.first), pair.second };");
            });
        });
        em.line();
        em.line("}");
    });
}
}

#endif  // PROTO11_CPP11_ENUM_HPP_INCLUDED