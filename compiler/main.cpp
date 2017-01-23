#include <google/protobuf/compiler/command_line_interface.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/compiler/python/python_generator.h>

#include "cpp11_generator.hpp"


int main(int argc, char** argv) {
    google::protobuf::compiler::CommandLineInterface cli;

    google::protobuf::compiler::cpp::CppGenerator cpp_gen;
    cli.RegisterGenerator("--cpp_out", &cpp_gen, "Generate C++98 Code");

    google::protobuf::compiler::python::Generator python_gen;
    cli.RegisterGenerator("--python_out", &python_gen, "Generate Python Code");

    proto11::Cpp11Generator cpp11_gen;
    cli.RegisterGenerator("--cpp11_out", &cpp11_gen, "Generator C++11 Code (proto11)");

    return cli.Run(argc, argv);
}