# Build edge case plugins as shared libraries

add_library(bu-empty-plugin SHARED
    empty_plugin.cpp
)
target_compile_definitions(bu-empty-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(bu-empty-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(bu-null-impl-plugin SHARED
    null_impl_plugin.cpp
)
target_compile_definitions(bu-null-impl-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(bu-null-impl-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(bu-special-names-plugin SHARED
    special_names_plugin.cpp
)
target_compile_definitions(bu-special-names-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(bu-special-names-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
