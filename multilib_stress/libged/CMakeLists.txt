# Build the GED library as a shared library with its own plugin system

add_library(ged_plugin_host SHARED
    ged_plugin_host.cpp
)

target_compile_definitions(ged_plugin_host PRIVATE 
    BU_PLUGIN_IMPLEMENTATION 
    BU_PLUGIN_BUILDING_DLL
    BU_PLUGIN_NAME=ged
)

target_include_directories(ged_plugin_host PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(WIN32)
    target_link_libraries(ged_plugin_host PRIVATE)
else()
    target_link_libraries(ged_plugin_host PRIVATE dl)
endif()
