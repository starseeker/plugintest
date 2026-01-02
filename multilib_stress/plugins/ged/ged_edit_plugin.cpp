/**
 * ged_edit_plugin.cpp - GED plugin for editing commands.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME ged
#endif
#include "bu_plugin.h"

static int ged_rotate(void) {
    printf("GED Plugin: rotate command executed\n");
    return 200;
}

static int ged_translate(void) {
    printf("GED Plugin: translate command executed\n");
    return 201;
}

static bu_plugin_cmd s_commands[] = {
    { "ged_rotate", ged_rotate },
    { "ged_translate", ged_translate }
};

static bu_plugin_manifest s_manifest = {
    "ged-edit-plugin",
    1,
    2,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
