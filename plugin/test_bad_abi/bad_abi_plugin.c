/**
 * bad_abi_plugin.c - Test plugin with incorrect ABI version.
 *
 * This plugin is used to test that the loader correctly rejects plugins
 * with mismatched abi_version fields.
 */

#include <stdio.h>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* Test command */
static int bad_abi_cmd(void) {
    printf("This command should never be called!\n");
    return 999;
}

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    { "bad_abi_cmd", bad_abi_cmd }
};

/* Define the manifest with INCORRECT abi_version (should be 1) */
static bu_plugin_manifest s_manifest = {
    "bu-bad-abi-plugin",        /* plugin_name */
    1,                          /* version */
    1,                          /* cmd_count */
    s_commands,                 /* commands */
    999,                        /* abi_version - INTENTIONALLY WRONG */
    sizeof(bu_plugin_manifest)  /* struct_size */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
