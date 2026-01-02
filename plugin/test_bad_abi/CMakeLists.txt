add_library(bu-bad-abi-plugin SHARED
    bad_abi_plugin.c
)
target_compile_definitions(bu-bad-abi-plugin PRIVATE BU_PLUGIN_BUILDING_DLL)
