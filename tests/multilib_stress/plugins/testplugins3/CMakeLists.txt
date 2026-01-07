# TestPlugins3 plugins

add_library(tp3-overlap-plugin SHARED tp3_overlap_plugin.cpp)
target_compile_definitions(tp3-overlap-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=testplugins3)
target_include_directories(tp3-overlap-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(tp3-volume-plugin SHARED tp3_volume_plugin.cpp)
target_compile_definitions(tp3-volume-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=testplugins3)
target_include_directories(tp3-volume-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
