/**
 * empty_plugin.cpp - Plugin with empty manifest.
 *
 * This plugin tests behavior when a plugin has no commands.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* Define the manifest with no commands */
static bu_plugin_manifest s_manifest = {
    "bu-empty-plugin",      /* plugin_name */
    1,                      /* version */
    0,                      /* cmd_count - no commands */
    nullptr                 /* commands - null array */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
