function(proto11_add_library name)
    set(sources)
    get_filename_component(gendir "${CMAKE_CURRENT_BINARY_DIR}/p11-gen" ABSOLUTE)
    foreach(input IN LISTS ARGN)
        get_filename_component(input "${input}" ABSOLUTE)
        file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}" "${input}")
        get_filename_component(generated_hpp "${gendir}/${relpath}.hpp" ABSOLUTE)
        get_filename_component(generated_cpp "${gendir}/${relpath}.cpp" ABSOLUTE)
        add_custom_command(
            OUTPUT "${generated_hpp}" "${generated_cpp}"
            DEPENDS "${input}" proto11
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${gendir}"
            COMMAND "${CMAKE_COMMAND}" -E chdir "${gendir}"
                $<TARGET_FILE:proto11>
                    -I "${CMAKE_CURRENT_SOURCE_DIR}"
                    --cpp11_out="${gendir}"
                    "${input}"
            COMMENT "Generating ${generated_hpp}"
        )
        list(APPEND sources "${generated_hpp}" "${generated_cpp}")
    endforeach()
    add_library(${name} ${sources})
    target_link_libraries(${name} PUBLIC proto11::encoder)
    target_include_directories(${name} PUBLIC $<BUILD_INTERFACE:${gendir}>)
endfunction()
