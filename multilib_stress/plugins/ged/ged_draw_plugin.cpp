/**
 * ged_draw_plugin.cpp - GED plugin for drawing commands.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME ged
#endif
#include "bu_plugin.h"

static int ged_draw(void) {
    printf("GED Plugin: draw command executed\n");
    return 100;
}

static int ged_erase(void) {
    printf("GED Plugin: erase command executed\n");
    return 101;
}

static int ged_make(void) {
    printf("GED Plugin: make command executed\n");
    return 102;
}

static bu_plugin_cmd s_commands[] = {
    { "ged_draw", ged_draw },
    { "ged_erase", ged_erase },
    { "ged_make", ged_make }
};

static bu_plugin_manifest s_manifest = {
    "ged-draw-plugin",
    1,
    3,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
