/**
 * tp2_render_plugin.cpp - TestPlugins2 plugin for rendering commands.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME testplugins2
#endif
#include "bu_plugin.h"

static int tp2_raytrace(void) {
    printf("TestPlugins2 Plugin: raytrace command executed\n");
    return 400;
}

static int tp2_preview(void) {
    printf("TestPlugins2 Plugin: preview command executed\n");
    return 401;
}

static bu_plugin_cmd s_commands[] = {
    { "tp2_raytrace", tp2_raytrace },
    { "tp2_preview", tp2_preview }
};

static bu_plugin_manifest s_manifest = {
    "tp2-render-plugin",
    1,
    2,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
