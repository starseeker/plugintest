/**
 * null_impl_plugin.cpp - Plugin with null command implementations.
 *
 * This plugin tests behavior when some commands have null implementations.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* A valid command */
static int valid_command(void) {
    printf("Null-impl plugin: valid command executed\n");
    return 1;
}

/* Define the command array with some null implementations */
static bu_plugin_cmd s_commands[] = {
    { "null_valid", valid_command },
    { "null_impl", nullptr },          /* Null implementation - should be skipped */
    { nullptr, valid_command },        /* Null name - should be skipped */
    { "null_both", nullptr }           /* Both aspects valid but no impl */
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "bu-null-impl-plugin",  /* plugin_name */
    1,                      /* version */
    4,                      /* cmd_count */
    s_commands              /* commands */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
