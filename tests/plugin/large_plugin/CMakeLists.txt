# Build the large plugin for scalability testing

add_library(bu-large-plugin SHARED
    large_plugin.cpp
)

# The plugin exports symbols
target_compile_definitions(bu-large-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)

# Include the project's include directory
target_include_directories(bu-large-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
