# TestPlugins1 plugins

add_library(tp1-draw-plugin SHARED tp1_draw_plugin.cpp)
target_compile_definitions(tp1-draw-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=testplugins1)
target_include_directories(tp1-draw-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(tp1-edit-plugin SHARED tp1_edit_plugin.cpp)
target_compile_definitions(tp1-edit-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=testplugins1)
target_include_directories(tp1-edit-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
