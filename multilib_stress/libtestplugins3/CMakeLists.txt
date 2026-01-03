# Build the TestPlugins3 library as a shared library with its own plugin system

add_library(testplugins3_plugin_host SHARED
    testplugins3_plugin_host.cpp
)

target_compile_definitions(testplugins3_plugin_host PRIVATE 
    BU_PLUGIN_IMPLEMENTATION 
    BU_PLUGIN_BUILDING_DLL
    BU_PLUGIN_NAME=testplugins3
)

target_include_directories(testplugins3_plugin_host PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(NOT WIN32)
    target_link_libraries(testplugins3_plugin_host PRIVATE dl)
endif()
