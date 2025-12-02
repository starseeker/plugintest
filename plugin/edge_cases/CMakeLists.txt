# Build edge case plugins as shared libraries

add_library(ged-empty-plugin SHARED
    empty_plugin.cpp
)
target_compile_definitions(ged-empty-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(ged-empty-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(ged-null-impl-plugin SHARED
    null_impl_plugin.cpp
)
target_compile_definitions(ged-null-impl-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(ged-null-impl-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(ged-special-names-plugin SHARED
    special_names_plugin.cpp
)
target_compile_definitions(ged-special-names-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
target_include_directories(ged-special-names-plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
