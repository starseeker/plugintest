# TestPlugins2 plugins

add_library(tp2-shader-plugin SHARED tp2_shader_plugin.cpp)
target_compile_definitions(tp2-shader-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=testplugins2)
target_include_directories(tp2-shader-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(tp2-render-plugin SHARED tp2_render_plugin.cpp)
target_compile_definitions(tp2-render-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=testplugins2)
target_include_directories(tp2-render-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
