/**
 * analyze_overlap_plugin.cpp - ANALYZE plugin for overlap analysis.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME analyze
#endif
#include "bu_plugin.h"

static int analyze_overlap(void) {
    printf("ANALYZE Plugin: overlap analysis executed\n");
    return 500;
}

static int analyze_gaps(void) {
    printf("ANALYZE Plugin: gaps analysis executed\n");
    return 501;
}

static int analyze_mass(void) {
    printf("ANALYZE Plugin: mass analysis executed\n");
    return 502;
}

static bu_plugin_cmd s_commands[] = {
    { "analyze_overlap", analyze_overlap },
    { "analyze_gaps", analyze_gaps },
    { "analyze_mass", analyze_mass }
};

static bu_plugin_manifest s_manifest = {
    "analyze-overlap-plugin",
    1,
    3,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
