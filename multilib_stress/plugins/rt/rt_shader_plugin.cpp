/**
 * rt_shader_plugin.cpp - RT plugin for shader commands.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME rt
#endif
#include "bu_plugin.h"

static int rt_phong(void) {
    printf("RT Plugin: phong shader executed\n");
    return 300;
}

static int rt_glass(void) {
    printf("RT Plugin: glass shader executed\n");
    return 301;
}

static int rt_mirror(void) {
    printf("RT Plugin: mirror shader executed\n");
    return 302;
}

static bu_plugin_cmd s_commands[] = {
    { "rt_phong", rt_phong },
    { "rt_glass", rt_glass },
    { "rt_mirror", rt_mirror }
};

static bu_plugin_manifest s_manifest = {
    "rt-shader-plugin",
    1,
    3,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
