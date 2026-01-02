# GED plugins

add_library(ged-draw-plugin SHARED ged_draw_plugin.cpp)
target_compile_definitions(ged-draw-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=ged)
target_include_directories(ged-draw-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(ged-edit-plugin SHARED ged_edit_plugin.cpp)
target_compile_definitions(ged-edit-plugin PRIVATE BU_PLUGIN_BUILDING_DLL BU_PLUGIN_NAME=ged)
target_include_directories(ged-edit-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
