/**
 * tp3_overlap_plugin.cpp - TestPlugins3 plugin for overlap analysis.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME testplugins3
#endif
#include "bu_plugin.h"

static int tp3_overlap(void) {
    printf("TestPlugins3 Plugin: overlap analysis executed\n");
    return 500;
}

static int tp3_gaps(void) {
    printf("TestPlugins3 Plugin: gaps analysis executed\n");
    return 501;
}

static int tp3_mass(void) {
    printf("TestPlugins3 Plugin: mass analysis executed\n");
    return 502;
}

static bu_plugin_cmd s_commands[] = {
    { "tp3_overlap", tp3_overlap },
    { "tp3_gaps", tp3_gaps },
    { "tp3_mass", tp3_mass }
};

static bu_plugin_manifest s_manifest = {
    "tp3-overlap-plugin",
    1,
    3,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
