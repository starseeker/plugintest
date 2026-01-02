/**
 * analyze_volume_plugin.cpp - ANALYZE plugin for volume analysis.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME analyze
#endif
#include "bu_plugin.h"

static int analyze_volume(void) {
    printf("ANALYZE Plugin: volume analysis executed\n");
    return 600;
}

static int analyze_surface(void) {
    printf("ANALYZE Plugin: surface analysis executed\n");
    return 601;
}

static bu_plugin_cmd s_commands[] = {
    { "analyze_volume", analyze_volume },
    { "analyze_surface", analyze_surface }
};

static bu_plugin_manifest s_manifest = {
    "analyze-volume-plugin",
    1,
    2,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
