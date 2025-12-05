/**
 * c_only_plugin.c - A pure C plugin for testing C-only plugin support.
 *
 * This plugin:
 *   - Is written entirely in C (no C++ features)
 *   - Tests that the plugin system works with pure C plugins
 *   - Verifies cross-platform compatibility (especially Windows)
 */

#include <stdio.h>

/* When building a plugin, we export symbols */
#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* C-only command implementation */
static int c_only_hello(void) {
    printf("Hello from the C-only plugin!\n");
    return 100;
}

/* Another C-only command */
static int c_only_goodbye(void) {
    printf("Goodbye from the C-only plugin!\n");
    return 200;
}

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    { "c_only_hello", c_only_hello },
    { "c_only_goodbye", c_only_goodbye }
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "bu-c-only-plugin",     /* plugin_name */
    1,                      /* version */
    2,                      /* cmd_count */
    s_commands              /* commands */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
