/**
 * tp2_shader_plugin.cpp - TestPlugins2 plugin for shader commands.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME testplugins2
#endif
#include "bu_plugin.h"

static int tp2_phong(void) {
    printf("TestPlugins2 Plugin: phong shader executed\n");
    return 300;
}

static int tp2_glass(void) {
    printf("TestPlugins2 Plugin: glass shader executed\n");
    return 301;
}

static int tp2_mirror(void) {
    printf("TestPlugins2 Plugin: mirror shader executed\n");
    return 302;
}

static bu_plugin_cmd s_commands[] = {
    { "tp2_phong", tp2_phong },
    { "tp2_glass", tp2_glass },
    { "tp2_mirror", tp2_mirror }
};

static bu_plugin_manifest s_manifest = {
    "tp2-shader-plugin",
    1,
    3,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
