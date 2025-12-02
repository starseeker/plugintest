# Build the stress plugin as a shared library

add_library(ged-stress-plugin SHARED
    stress_plugin.cpp
)

target_compile_definitions(ged-stress-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(ged-stress-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
