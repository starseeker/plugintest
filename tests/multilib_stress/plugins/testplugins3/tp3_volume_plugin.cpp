/**
 * tp3_volume_plugin.cpp - TestPlugins3 plugin for volume analysis.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME testplugins3
#endif
#include "bu_plugin.h"

static int tp3_volume(void) {
    printf("TestPlugins3 Plugin: volume analysis executed\n");
    return 600;
}

static int tp3_surface(void) {
    printf("TestPlugins3 Plugin: surface analysis executed\n");
    return 601;
}

static bu_plugin_cmd s_commands[] = {
    { "tp3_volume", tp3_volume },
    { "tp3_surface", tp3_surface }
};

static bu_plugin_manifest s_manifest = {
    "tp3-volume-plugin",
    1,
    2,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
