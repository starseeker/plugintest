/**
 * tp1_draw_plugin.cpp - TestPlugins1 plugin for drawing commands.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME testplugins1
#endif
#include "bu_plugin.h"

static int tp1_draw(void) {
    printf("TestPlugins1 Plugin: draw command executed\n");
    return 100;
}

static int tp1_erase(void) {
    printf("TestPlugins1 Plugin: erase command executed\n");
    return 101;
}

static int tp1_make(void) {
    printf("TestPlugins1 Plugin: make command executed\n");
    return 102;
}

static bu_plugin_cmd s_commands[] = {
    { "tp1_draw", tp1_draw },
    { "tp1_erase", tp1_erase },
    { "tp1_make", tp1_make }
};

static bu_plugin_manifest s_manifest = {
    "tp1-draw-plugin",
    1,
    3,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
