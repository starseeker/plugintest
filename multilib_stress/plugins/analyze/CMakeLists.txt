# ANALYZE plugins

add_library(analyze-overlap-plugin SHARED analyze_overlap_plugin.cpp)
target_compile_definitions(analyze-overlap-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=analyze)
target_include_directories(analyze-overlap-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(analyze-volume-plugin SHARED analyze_volume_plugin.cpp)
target_compile_definitions(analyze-volume-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=analyze)
target_include_directories(analyze-volume-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
