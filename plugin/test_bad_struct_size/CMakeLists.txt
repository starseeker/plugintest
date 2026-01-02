add_library(bu-bad-struct-plugin SHARED
    bad_struct_size_plugin.c
)
target_compile_definitions(bu-bad-struct-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
