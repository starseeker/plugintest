cmake_minimum_required(VERSION 3.14)
project(plugintest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Option for strict warnings (to test compilation with stricter settings)
option(ENABLE_STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(ENABLE_WERROR "Treat warnings as errors" OFF)
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

# Compiler-specific warning flags
if(ENABLE_STRICT_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wcast-align
            -Wcast-qual
            -Wconversion
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
            -Wmissing-include-dirs
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wredundant-decls
            -Wshadow
            -Wsign-conversion
            -Wundef
            -Wunused
        )
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            add_compile_options(
                -Wlogical-op
                -Wduplicated-cond
                -Wduplicated-branches
            )
        endif()
    elseif(MSVC)
        add_compile_options(/W4)
    endif()
endif()

if(ENABLE_WERROR)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Werror)
    elseif(MSVC)
        add_compile_options(/WX)
    endif()
endif()

# Sanitizer support
if(ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Ensure shared libraries use RPATH (for dynamic loading)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Enable testing
enable_testing()

# Host library (shared) with built-in implementation
add_library(ged_host SHARED
    host/libged_init.cpp
)
target_compile_definitions(ged_host PRIVATE BU_PLUGIN_IMPLEMENTATION)
if(WIN32)
    target_link_libraries(ged_host PRIVATE)
else()
    target_link_libraries(ged_host PRIVATE dl)
endif()

# Executable that loads plugins and runs commands
add_executable(run_host
    host/exec.cpp
)
target_link_libraries(run_host PRIVATE ged_host)

# Plugin subdirectories
add_subdirectory(plugin/example)
add_subdirectory(plugin/math_plugin)
add_subdirectory(plugin/string_plugin)
add_subdirectory(plugin/duplicate_plugin)
add_subdirectory(plugin/stress_plugin)
add_subdirectory(plugin/edge_cases)

# Tests
add_subdirectory(tests)
