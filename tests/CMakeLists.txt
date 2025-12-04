# Tests CMakeLists.txt

# Test harness executable
add_executable(test_harness
    test_harness.cpp
)
target_link_libraries(test_harness PRIVATE ged_host)
target_include_directories(test_harness PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add test target using CTest
# Pass the build directory and configuration as arguments
# For multi-config generators (like Visual Studio), $<CONFIG> resolves to the actual config
add_test(NAME plugin_tests 
    COMMAND test_harness ${CMAKE_BINARY_DIR} $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
