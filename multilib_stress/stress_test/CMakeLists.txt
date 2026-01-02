# Multi-library stress test executable

add_executable(multilib_stress_test
    multilib_stress_test.cpp
)

# Link against dl on Unix systems for dynamic loading
if(NOT WIN32)
    target_link_libraries(multilib_stress_test PRIVATE dl)
endif()

target_include_directories(multilib_stress_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add as a test
add_test(NAME multilib_stress_test COMMAND multilib_stress_test)
