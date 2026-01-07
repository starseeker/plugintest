# Build the math plugin as a shared library

add_library(bu-math-plugin SHARED
    math_plugin.cpp
)

target_compile_definitions(bu-math-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(bu-math-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
