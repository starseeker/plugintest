# Build the duplicate plugin as a shared library

add_library(ged-duplicate-plugin SHARED
    duplicate_plugin.cpp
)

target_compile_definitions(ged-duplicate-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(ged-duplicate-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
