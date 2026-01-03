# Build the TestPlugins2 library as a shared library with its own plugin system

add_library(testplugins2_plugin_host SHARED
    testplugins2_plugin_host.cpp
)

target_compile_definitions(testplugins2_plugin_host PRIVATE 
    BU_PLUGIN_IMPLEMENTATION 
    BU_PLUGIN_BUILDING_DLL
    BU_PLUGIN_NAME=testplugins2
)

target_include_directories(testplugins2_plugin_host PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(NOT WIN32)
    target_link_libraries(testplugins2_plugin_host PRIVATE dl)
endif()
