# Build the RT library as a shared library with its own plugin system

add_library(rt_plugin_host SHARED
    rt_plugin_host.cpp
)

target_compile_definitions(rt_plugin_host PRIVATE 
    BU_PLUGIN_IMPLEMENTATION 
    BU_PLUGIN_BUILDING_DLL
    BU_PLUGIN_NAME=rt
)

target_include_directories(rt_plugin_host PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(WIN32)
    target_link_libraries(rt_plugin_host PRIVATE)
else()
    target_link_libraries(rt_plugin_host PRIVATE dl)
endif()
