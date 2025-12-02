cmake_minimum_required(VERSION 3.14)
project(plugintest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure shared libraries use RPATH (for dynamic loading)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

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

# Plugin subdirectory
add_subdirectory(plugin/example)
