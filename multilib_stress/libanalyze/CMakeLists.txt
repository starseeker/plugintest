# Build the ANALYZE library as a shared library with its own plugin system

add_library(analyze_plugin_host SHARED
    analyze_plugin_host.cpp
)

target_compile_definitions(analyze_plugin_host PRIVATE 
    BU_PLUGIN_IMPLEMENTATION 
    BU_PLUGIN_BUILDING_DLL
    BU_PLUGIN_NAME=analyze
)

target_include_directories(analyze_plugin_host PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(WIN32)
    target_link_libraries(analyze_plugin_host PRIVATE)
else()
    target_link_libraries(analyze_plugin_host PRIVATE dl)
endif()
