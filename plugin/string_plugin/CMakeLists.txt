# Build the string plugin as a shared library

add_library(ged-string-plugin SHARED
    string_plugin.cpp
)

target_compile_definitions(ged-string-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(ged-string-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
