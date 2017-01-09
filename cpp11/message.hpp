#ifndef PROTO11_MESSAGE_HPP_INCLUDED
#define PROTO11_MESSAGE_HPP_INCLUDED

#include <google/protobuf/Descriptor.h>

#include "helpers.hpp"

#include "emitter.hpp"
#include "enum.hpp"

#include <proto11/encoder/message.hpp>

namespace proto11 {

inline std::string field_base_type_name(const google::protobuf::FieldDescriptor& field) {
    switch (field.type()) {
#define X(tn, tns)                                                                                                     \
    case google::protobuf::FieldDescriptor::TYPE_##tn:                                                                 \
        return tns
        X(DOUBLE, "double");
        X(FLOAT, "float");
        X(INT64, "std::int64_t");
        X(UINT64, "std::uint64_t");
        X(INT32, "std::int32_t");
        X(FIXED64, "std::int64_t");
        X(FIXED32, "std::int32_t");
        X(BOOL, "bool");
        X(STRING, "std::string");
        // X(GROUP, )
        X(MESSAGE, qualified_name(*field.message_type()));
        X(BYTES, "std::vector<std::uint8_t>");
        X(UINT32, "std::uint32_t");
        X(ENUM, qualified_name(*field.enum_type()));
        X(SFIXED32, "std::int32_t");
        X(SFIXED64, "std::int64_t");
        X(SINT32, "std::int32_t");
        X(SINT64, "std::int64_t");
#undef X
    default:
        std::terminate();
    }
}

inline std::string field_type_name(const google::protobuf::FieldDescriptor& field) {
    std::string base_name = field_base_type_name(field);
    switch (field.label()) {
    case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
        return base_name;
    case google::protobuf::FieldDescriptor::LABEL_REPEATED:
        return "std::vector<" + base_name + ">";
    case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
        return "boost::optional<" + base_name + ">";
    default:
        std::terminate();
    }
}

inline wire_type field_wire_type(const google::protobuf::FieldDescriptor& field) {
#define T(tn) google::protobuf::FieldDescriptor::Type::TYPE_##tn
    switch (field.type()) {
    case T(FIXED32):
    case T(SFIXED32):
    case T(FLOAT):
        return wire_type::fixed32;
    case T(FIXED64):
    case T(SFIXED64):
    case T(DOUBLE):
        return wire_type::fixed64;
    case T(BOOL):
    case T(ENUM):
    case T(INT32):
    case T(INT64):
    case T(SINT32):
    case T(SINT64):
    case T(UINT32):
    case T(UINT64):
        return wire_type::varint;
    case T(BYTES):
    case T(MESSAGE):
    case T(STRING):
        return wire_type::length_delimited;
    }
    std::terminate();
}

inline void emit_field_writer(Emitter& em, std::string accessor, const google::protobuf::FieldDescriptor& field) {
    em.line("header.field = ", field.number(), ';');
    std::string htype;
#define T(tn) google::protobuf::FieldDescriptor::Type::TYPE_##tn
    switch (field.type()) {
    case T(FIXED32):
    case T(SFIXED32):
    case T(FLOAT):
        htype = "fixed32";
        break;
    case T(FIXED64):
    case T(SFIXED64):
    case T(DOUBLE):
        htype = "fixed64";
        em.line("header.type = wire_type::fixed64;");
        break;
    case T(BOOL):
    case T(ENUM):
    case T(INT32):
    case T(INT64):
    case T(SINT32):
    case T(SINT64):
    case T(UINT32):
    case T(UINT64):
        htype = "varint";
        break;
    case T(BYTES):
    case T(MESSAGE):
    case T(STRING):
        htype = "length_delimited";
        break;
    }
    em.line("header.type = wire_type::", htype, ';');
    em.line("iter = detail::write_field_header(header, iter);");

    switch (field.type()) {
    case T(FIXED64):
    case T(FIXED32):
    case T(SFIXED32):
    case T(SFIXED64):
    case T(DOUBLE):
    case T(FLOAT):
        em.line("iter = proto11::write_fixed(", accessor, ", iter);");
        break;
    case T(ENUM):
    case T(INT32):
    case T(INT64):
    case T(UINT32):
    case T(UINT64):
        em.line("iter = write_varint(static_cast<std::uint64_t>(", accessor, "), iter);");
        break;
    case T(BOOL):
        em.line("iter = write_varint(", accessor, " ? 1u : 0u, iter);");
        break;
    case T(BYTES):
    case T(STRING):
        em.line("iter = write_varint(", accessor, ".size(), iter);");
        em.line("iter = std::copy(", accessor, ".begin(), ", accessor, ".end(), iter);");
        break;
    case T(MESSAGE):
        em.block("{", "}", [&] {
            em.line("std::string buffer;");
            em.line("buffer.reserve(256);");
            em.line("proto11::write(", accessor, ", std::back_inserter(buffer));");
            em.line("iter = write_varint(buffer.size(), iter);");
            em.line("iter = std::copy(buffer.begin(), buffer.end(), iter);");
        });
        break;
    case T(SINT32):
    case T(SINT64):
        em.line("iter = write_varint(zigzag_encode(", accessor, "), iter);");
        break;
    }

#undef T
}

inline void emit_message(Emitter& em, const google::protobuf::Descriptor& desc) {
    em.line("struct ", desc.name(), " {");
    em.indented([&] {
        PROTO11_FOREACH(desc, enum_type) {
            emit_enum(em, enum_type);
            em.line();
        };
        PROTO11_FOREACH(desc, nested_type) {
            emit_message(em, nested_type);
            em.line();
        };

        PROTO11_FOREACH(desc, field) { em.line(field_type_name(field), ' ', field.name(), ';'); };
    });
    em.line("};");

    em.tail([&] {
        em.line("namespace proto11 {");
        em.line();
        em.block("template <> struct traits<" + qualified_name(desc) + ", void> {", "};", [&] {
            em.block("enum {", "};", [&] {
                em.line("valid = true,");
                em.line("is_enum = false,");
                em.line("is_message = true,");
            });
            em.line("using check = std::true_type;");
            em.line("using type = ", qualified_name(desc), ';');

            // This is the definition of the writer
            em.block("template <typename Iter> static Iter write(const type& msg, Iter iter) {", "}", [&] {
                em.line("detail::field_header header;");
                PROTO11_FOREACH(desc, field) {
                    em.line("// Emit field: ", field.name());
                    std::string accessor = "msg." + field.name();
                    if (field.label() == google::protobuf::FieldDescriptor::LABEL_REQUIRED) {
                        emit_field_writer(em, accessor, field);
                    } else if (field.label() == google::protobuf::FieldDescriptor::LABEL_OPTIONAL) {
                        em.block("if (" + accessor + ") {", "}", [&] {
                            emit_field_writer(em, "(*" + accessor + ")", field);
                        });
                    } else if (field.label() == google::protobuf::FieldDescriptor::LABEL_REPEATED) {
                        em.block("for (const auto& item : " + accessor + ") {", "}", [&] {
                            emit_field_writer(em, "item", field);
                        });
                    }
                };
                em.line("return iter;");
            });

            // And this is the reader
            em.block("template <typename Iter> static std::pair<type, Iter> read(Iter iter, const Iter last) {",
                     "}",
                     [&] {
                         em.line("type ret;");
                         em.block("while (iter != last) {", "}", [&] {
                             em.line("detail::field_header header;");
                             em.line("std::tie(header, iter) = detail::read_field_header(iter, last);");
                             em.line("switch(header.field) {");
                             PROTO11_FOREACH(desc, field) {
                                 em.line("// Decode field ", field.name());
                                 em.block("case " + std::to_string(field.number()) + ": {", "}", [&] {
                                     if (field_wire_type(field) == wire_type::length_delimited) {
                                         em.line("std::tie(ret.",
                                                 field.name(),
                                                 ", iter) = proto11::detail::read_length_delim<",
                                                 field_base_type_name(field),
                                                 ">(iter, last, typename "
                                                 "std::iterator_traits<Iter>::iterator_category{});");
                                     } else {
                                         em.line("std::tie(ret.",
                                                 field.name(),
                                                 ", iter) = proto11::detail::read_integral<",
                                                 field_base_type_name(field),
                                                 ">(header, iter, last);");
                                     }
                                     em.line("break;");
                                 });
                             };
                             em.line('}');
                         });
                         em.line("return { ret, iter };");
                     });
        });
        em.line();
        PROTO11_FOREACH(desc, field) {
            em.line("template <>");
            em.block("struct field_traits<" + field_type_name(field) + " " + qualified_name(desc) + "::*, &"
                         + qualified_name(desc)
                         + "::"
                         + field.name()
                         + "> {",
                     "};",
                     [&] {
                         em.line("using type                    = ", field_type_name(field), ';');
                         em.line("using containing_type         = ", qualified_name(desc), ';');
                         em.line("static constexpr auto number  = ", field.number(), ';');
                         em.line("static constexpr auto pointer = &", qualified_name(desc), "::", field.name(), ';');
                         em.line("static constexpr auto name    = \"", field.name(), "\";");
                     });
            em.line();
        };
        em.line("}");
    });
}

}  // namespace proto11

#endif  // PROTO11_MESSAGE_HPP_INCLUDED