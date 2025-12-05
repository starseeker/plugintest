# Build the C-only plugin as a shared library
# This plugin is written entirely in C to test cross-platform C plugin support

add_library(bu-c-only-plugin SHARED
    c_only_plugin.c
)

# Set the C standard for this target
set_target_properties(bu-c-only-plugin PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# The plugin exports symbols
target_compile_definitions(bu-c-only-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)

target_include_directories(bu-c-only-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

# On Windows, plugins don't need to link against the host library
# since they use the manifest pattern (no direct registry calls from plugin)
