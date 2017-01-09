#ifndef PROTO11_HELPERS_HPP_INCLUDED
#define PROTO11_HELPERS_HPP_INCLUDED

#include <string>

#include <google/protobuf/descriptor.h>

namespace proto11 {

inline std::string qualified_name(std::string str) {
    std::string acc = str;
    std::size_t pos = 0;
    while ((pos = acc.find('.')) != acc.npos)
        acc.replace(pos, 1, "::");
    return acc;
}

template <typename Desc>
std::string qualified_name(const Desc& desc) {
    return qualified_name(desc.full_name());
}


}  // namespace proto11

#endif  // PROTO11_HELPERS_HPP_INCLUDED