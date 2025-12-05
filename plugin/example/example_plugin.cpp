/**
 * example_plugin.cpp - A trivial example plugin.
 *
 * This plugin:
 *   - Implements one command named "example" with signature int (*)(void)
 *   - Exports a manifest via bu_plugin_info
 */

#include <cstdio>

/* When building a plugin, we export symbols */
#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* The example command implementation */
static int example_command(void) {
    printf("Hello from the example plugin!\n");
    return 42;
}

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    { "example", example_command }
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "bu-example-plugin",    /* plugin_name */
    1,                      /* version */
    1,                      /* cmd_count */
    s_commands,             /* commands */
    BU_PLUGIN_ABI_VERSION,  /* abi_version */
    sizeof(bu_plugin_manifest) /* struct_size */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
