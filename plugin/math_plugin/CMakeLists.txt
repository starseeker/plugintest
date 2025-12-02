# Build the math plugin as a shared library

add_library(ged-math-plugin SHARED
    math_plugin.cpp
)

target_compile_definitions(ged-math-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(ged-math-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
