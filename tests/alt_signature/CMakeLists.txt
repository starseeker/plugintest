# Test for alternative function signatures

add_executable(test_alt_signature
    test_alt_signature.cpp
)

target_include_directories(test_alt_signature PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link against dl on Unix systems
if(NOT WIN32)
    target_link_libraries(test_alt_signature PRIVATE dl)
endif()

# Add as a test
add_test(
    NAME test_alt_signature
    COMMAND test_alt_signature
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
