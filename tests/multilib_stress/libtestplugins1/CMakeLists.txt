# Build the TestPlugins1 library as a shared library with its own plugin system

add_library(testplugins1_plugin_host SHARED
    testplugins1_plugin_host.cpp
)

target_compile_definitions(testplugins1_plugin_host PRIVATE 
    BU_PLUGIN_IMPLEMENTATION 
    BU_PLUGIN_BUILDING_DLL
    BU_PLUGIN_NAME=testplugins1
)

target_include_directories(testplugins1_plugin_host PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(NOT WIN32)
    target_link_libraries(testplugins1_plugin_host PRIVATE dl)
endif()
