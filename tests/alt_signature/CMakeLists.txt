# Test for alternative function signatures

# Host library for alternative signature commands
add_library(alt_sig_host SHARED
    host/alt_sig_host.cpp
)
target_compile_definitions(alt_sig_host PRIVATE BU_PLUGIN_IMPLEMENTATION BU_PLUGIN_BUILDING_DLL)
target_include_directories(alt_sig_host PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(alt_sig_host PRIVATE dl)
endif()

# Args plugin - built as SHARED (not MODULE) for cross-platform compatibility
add_library(alt-args-plugin SHARED
    plugins/args_plugin.cpp
)
target_compile_definitions(alt-args-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(alt-args-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

# String operations plugin - built as SHARED (not MODULE) for cross-platform compatibility
add_library(alt-string-ops-plugin SHARED
    plugins/string_ops_plugin.cpp
)
target_compile_definitions(alt-string-ops-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(alt-string-ops-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Test executable
add_executable(test_alt_signature
    test_alt_signature.cpp
)

target_include_directories(test_alt_signature PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_alt_signature PRIVATE alt_sig_host)

# Ensure plugins are built before the test
add_dependencies(test_alt_signature alt-args-plugin alt-string-ops-plugin)

# Add as a test
# Pass the build directory and configuration as arguments (for multi-config generators)
add_test(
    NAME test_alt_signature
    COMMAND test_alt_signature ${CMAKE_BINARY_DIR} $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
