#ifndef PROTO11_CPP11_GENERATOR_HPP_INCLUDED
#define PROTO11_CPP11_GENERATOR_HPP_INCLUDED

#include "emitter.hpp"

#include <google/protobuf/compiler/code_generator.h>

namespace proto11 {

class Cpp11Generator : public google::protobuf::compiler::CodeGenerator {
    bool Generate(const google::protobuf::FileDescriptor*,
                  const std::string&,
                  google::protobuf::compiler::GeneratorContext*,
                  std::string*) const override;
};
}

#endif  // PROTO11_CPP11_GENERATOR_HPP_INCLUDED