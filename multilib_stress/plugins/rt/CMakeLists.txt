# RT plugins

add_library(rt-shader-plugin SHARED rt_shader_plugin.cpp)
target_compile_definitions(rt-shader-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=rt)
target_include_directories(rt-shader-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(rt-render-plugin SHARED rt_render_plugin.cpp)
target_compile_definitions(rt-render-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=rt)
target_include_directories(rt-render-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
