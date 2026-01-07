# Tests CMakeLists.txt

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
