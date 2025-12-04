# Build the example plugin as a shared library

add_library(bu-example-plugin SHARED
    example_plugin.cpp
)

# The plugin exports symbols
target_compile_definitions(bu-example-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)

# Include the project's include directory
target_include_directories(bu-example-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

# On Windows, plugins don't need to link against the host library
# since they use the manifest pattern (no direct registry calls from plugin)
