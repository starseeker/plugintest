# Tests CMakeLists.txt
# This directory contains all test-related code including host components and plugins

# Host library (shared) with built-in implementation
add_library(bu_plugin_host SHARED
    host/libbu_init.cpp
)
target_compile_definitions(bu_plugin_host PRIVATE BU_PLUGIN_IMPLEMENTATION BU_PLUGIN_BUILDING_DLL)
if(WIN32)
    target_link_libraries(bu_plugin_host PRIVATE)
else()
    target_link_libraries(bu_plugin_host PRIVATE dl)
endif()

# Executable that loads plugins and runs commands
add_executable(run_bu_plugin
    host/exec.cpp
)
target_link_libraries(run_bu_plugin PRIVATE bu_plugin_host)

# Plugin subdirectories
add_subdirectory(plugin/example)
add_subdirectory(plugin/math_plugin)
add_subdirectory(plugin/string_plugin)
add_subdirectory(plugin/duplicate_plugin)
add_subdirectory(plugin/stress_plugin)
add_subdirectory(plugin/large_plugin)
add_subdirectory(plugin/edge_cases)
add_subdirectory(plugin/c_only)

# Test harness executable
add_executable(test_harness
    test_harness.cpp
)
target_link_libraries(test_harness PRIVATE bu_plugin_host)
target_include_directories(test_harness PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Robustness test executable
add_executable(test_robustness
    test_robustness.cpp
)
target_link_libraries(test_robustness PRIVATE bu_plugin_host)
target_include_directories(test_robustness PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link pthread for std::thread support on Linux
if(NOT WIN32)
    find_package(Threads REQUIRED)
    target_link_libraries(test_robustness PRIVATE Threads::Threads)
endif()

# Add test target using CTest
# Pass the build directory and configuration as arguments
# For multi-config generators (like Visual Studio), $<CONFIG> resolves to the actual config
add_test(NAME plugin_tests 
    COMMAND test_harness ${CMAKE_BINARY_DIR} $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_test(NAME robustness_tests 
    COMMAND test_robustness ${CMAKE_BINARY_DIR} $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Alternative signature test
add_subdirectory(alt_signature)

# Test-only plugins for ABI validation
add_subdirectory(plugins/test_bad_abi)
add_subdirectory(plugins/test_bad_struct_size)
add_subdirectory(plugins/test_no_manifest)

# Multi-library stress test (tests multiple independent libraries with separate plugin ecosystems)
add_subdirectory(multilib_stress)

# Test the run_bu_plugin executable with various plugins
# This ensures complete coverage of the host executable functionality

# Test run_bu_plugin with example plugin
add_test(NAME run_bu_plugin_example
    COMMAND run_bu_plugin $<TARGET_FILE:bu-example-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_example PROPERTIES
    PASS_REGULAR_EXPRESSION "Command 'example' returned: 42"
)

# Test run_bu_plugin with math plugin
add_test(NAME run_bu_plugin_math
    COMMAND run_bu_plugin $<TARGET_FILE:bu-math-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_math PROPERTIES
    PASS_REGULAR_EXPRESSION "Registered 3 command\\(s\\)"
)

# Test run_bu_plugin with string plugin
add_test(NAME run_bu_plugin_string
    COMMAND run_bu_plugin $<TARGET_FILE:bu-string-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_string PROPERTIES
    PASS_REGULAR_EXPRESSION "Registered 2 command\\(s\\)"
)

# Test run_bu_plugin with c_only plugin
add_test(NAME run_bu_plugin_c_only
    COMMAND run_bu_plugin $<TARGET_FILE:bu-c-only-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_c_only PROPERTIES
    PASS_REGULAR_EXPRESSION "Registered 2 command\\(s\\)"
)

# Test run_bu_plugin with stress plugin
add_test(NAME run_bu_plugin_stress
    COMMAND run_bu_plugin $<TARGET_FILE:bu-stress-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_stress PROPERTIES
    PASS_REGULAR_EXPRESSION "Registered 50 command\\(s\\)"
)

# Test run_bu_plugin with large plugin
add_test(NAME run_bu_plugin_large
    COMMAND run_bu_plugin $<TARGET_FILE:bu-large-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_large PROPERTIES
    PASS_REGULAR_EXPRESSION "Registered 500 command\\(s\\)"
)

# Test run_bu_plugin with duplicate plugin
add_test(NAME run_bu_plugin_duplicate
    COMMAND run_bu_plugin $<TARGET_FILE:bu-duplicate-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_duplicate PROPERTIES
    PASS_REGULAR_EXPRESSION "Registered [0-9]+ command\\(s\\)"
)

# Test run_bu_plugin with empty plugin
add_test(NAME run_bu_plugin_empty
    COMMAND run_bu_plugin $<TARGET_FILE:bu-empty-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_empty PROPERTIES
    PASS_REGULAR_EXPRESSION "Registered 0 command\\(s\\)"
)

# Test run_bu_plugin with null implementation plugin
add_test(NAME run_bu_plugin_null_impl
    COMMAND run_bu_plugin $<TARGET_FILE:bu-null-impl-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_null_impl PROPERTIES
    PASS_REGULAR_EXPRESSION "Registered [0-9]+ command\\(s\\)"
)

# Test run_bu_plugin with special names plugin
add_test(NAME run_bu_plugin_special_names
    COMMAND run_bu_plugin $<TARGET_FILE:bu-special-names-plugin>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_special_names PROPERTIES
    PASS_REGULAR_EXPRESSION "Registered 4 command\\(s\\)"
)

# Test run_bu_plugin without arguments (no plugin loaded)
add_test(NAME run_bu_plugin_no_plugin
    COMMAND run_bu_plugin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(run_bu_plugin_no_plugin PROPERTIES
    PASS_REGULAR_EXPRESSION "Command 'example' not registered"
)
