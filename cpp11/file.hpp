#ifndef PROTO11_CPP11_FILE_HPP_INCLUDED
#define PROTO11_CPP11_FILE_HPP_INCLUDED

#include "emitter.hpp"
#include "foreach.hpp"
#include "enum.hpp"
#include "message.hpp"

#include <google/protobuf/descriptor.h>

#include <algorithm>

namespace proto11 {

inline void emit_file(Emitter& em, const google::protobuf::FileDescriptor& file) {
    // Start by including some proto11 stuff
    em.headers([&] {
        em.line("#include <proto11/generated.hpp>");
        em.line();
        em.line("#include <type_traits>");
        em.line("#include <boost/optional.hpp>");
    });

    // Emit the namespace directives
    const auto ns_depth = count(file.package().begin(), file.package().end(), '.') + 1;
    auto ns_prefix = file.package();
    std::size_t dot_pos = 0;
    while ((dot_pos = ns_prefix.find('.')) != ns_prefix.npos)
        ns_prefix.replace(dot_pos, 1, " { namespace ");
    if (!file.package().empty()) {
        em.line("namespace ", ns_prefix, " {");
    }
    em.line();

    PROTO11_FOREACH(file, enum_type) {
        emit_enum(em, enum_type);
        em.line();
    };

    // Forward declare each message
    PROTO11_FOREACH(file, message_type) {
        em.line("struct ", message_type.name(), ';');
    };
    em.line();

    PROTO11_FOREACH(file, message_type) {
        emit_message(em, message_type);
        em.line();
    };

    // Close any namespace directives we may have opened
    if (!file.package().empty()) {
        em.line(std::string(ns_depth, '}'));
    }
}

}

#endif // PROTO11_CPP11_FILE_HPP_INCLUDED