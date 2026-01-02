/**
 * tp1_edit_plugin.cpp - TestPlugins1 plugin for editing commands.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME testplugins1
#endif
#include "bu_plugin.h"

static int tp1_rotate(void) {
    printf("TestPlugins1 Plugin: rotate command executed\n");
    return 200;
}

static int tp1_translate(void) {
    printf("TestPlugins1 Plugin: translate command executed\n");
    return 201;
}

static bu_plugin_cmd s_commands[] = {
    { "tp1_rotate", tp1_rotate },
    { "tp1_translate", tp1_translate }
};

static bu_plugin_manifest s_manifest = {
    "tp1-edit-plugin",
    1,
    2,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
