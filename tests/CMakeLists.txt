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

# Build configuration test - tests various CMake build configurations
# This replicates the functionality of test_builds.sh but uses CMake script
# Note: This test creates separate build directories and can take significant time
add_test(NAME build_config_tests
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/test_builds.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Alternative signature test
add_subdirectory(alt_signature)

# Test-only plugins for ABI validation
add_subdirectory(plugins/test_bad_abi)
add_subdirectory(plugins/test_bad_struct_size)
add_subdirectory(plugins/test_no_manifest)

# Multi-library stress test (tests multiple independent libraries with separate plugin ecosystems)
add_subdirectory(multilib_stress)
