#include "cpp11_generator.hpp"

#include "file.hpp"

#include <iostream>
#include <fstream>

bool proto11::Cpp11Generator::Generate(const google::protobuf::FileDescriptor* file,
                                       const std::string&,
                                       google::protobuf::compiler::GeneratorContext* ct,
                                       std::string* error) const {
    try {
        Emitter em;
        emit_file(em, *file);
        std::ofstream ostream{ file->name() + ".hpp" };
        ostream << em.get_string();
        std::ofstream cpp_ostream{ file->name() + ".cpp" };
        cpp_ostream << "#include \"" + file->name() + ".hpp\"\n\n" + em.get_cpp_string();
        return true;
    } catch (const std::runtime_error& e) {
        *error = e.what();
        return false;
    }
}