cmake_minimum_required(VERSION 3.7.0)
project(proto11 VERSION 0.0.0)

find_package(protobuf 3.1.0 CONFIG REQUIRED)
find_package(Boost 1.62 REQUIRED)

include(CMakeToolsHelpers OPTIONAL)

add_library(encoder
    proto11/generated.hpp
    proto11/encoder/encoder.cpp
    proto11/encoder/encoder.hpp
    proto11/encoder/varint.hpp
    )
add_library(proto11::encoder ALIAS encoder)
target_include_directories(encoder PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(encoder PUBLIC Boost::boost)

add_executable(proto11
    main.cpp

    cpp11/emitter.hpp
    cpp11/cpp11_generator.hpp
    cpp11/cpp11_generator.cpp
    )
target_link_libraries(proto11 PRIVATE protobuf::libprotoc proto11::encoder)

if(MSVC)
    target_compile_options(proto11 PRIVATE /MT$<$<CONFIG:Debug>:d>)
endif()

include(TestBigEndian)
test_big_endian(IS_BIG_ENDIAN)
target_compile_definitions(encoder PUBLIC PROTO11_IS_BIG_ENDIAN=${IS_BIG_ENDIAN})

include(CTest)
enable_testing()

include(generate.cmake)

if(BUILD_TESTING)
    include(tests.cmake)
endif()