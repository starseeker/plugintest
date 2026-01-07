/**
 * bad_struct_size_plugin.c - Test plugin with incorrect struct_size.
 *
 * This plugin is used to test that the loader correctly rejects plugins
 * with too-small struct_size fields.
 */

#include <stdio.h>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* Test command */
static int bad_struct_cmd(void) {
    printf("This command should never be called!\n");
    return 888;
}

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    { "bad_struct_cmd", bad_struct_cmd }
};

/* Define the manifest with INCORRECT struct_size (too small) */
static bu_plugin_manifest s_manifest = {
    "bu-bad-struct-plugin",     /* plugin_name */
    1,                          /* version */
    1,                          /* cmd_count */
    s_commands,                 /* commands */
    BU_PLUGIN_ABI_VERSION,      /* abi_version */
    1                           /* struct_size - INTENTIONALLY TOO SMALL */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
