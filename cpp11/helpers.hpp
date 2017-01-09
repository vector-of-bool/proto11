#ifndef PROTO11_HELPERS_HPP_INCLUDED
#define PROTO11_HELPERS_HPP_INCLUDED

#include <string>

#include <google/protobuf/descriptor.h>

namespace proto11 {

template <typename Desc>
std::string qualified_name(const Desc& desc) {
    std::string acc = desc.full_name();
    std::size_t pos = 0;
    while ((pos = acc.find('.')) != acc.npos)
        acc.replace(pos, 1, "::");
    return acc;
}


}  // namespace proto11

#endif  // PROTO11_HELPERS_HPP_INCLUDED