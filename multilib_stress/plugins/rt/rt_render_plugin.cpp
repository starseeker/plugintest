/**
 * rt_render_plugin.cpp - RT plugin for rendering commands.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME rt
#endif
#include "bu_plugin.h"

static int rt_raytrace(void) {
    printf("RT Plugin: raytrace command executed\n");
    return 400;
}

static int rt_preview(void) {
    printf("RT Plugin: preview command executed\n");
    return 401;
}

static bu_plugin_cmd s_commands[] = {
    { "rt_raytrace", rt_raytrace },
    { "rt_preview", rt_preview }
};

static bu_plugin_manifest s_manifest = {
    "rt-render-plugin",
    1,
    2,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
