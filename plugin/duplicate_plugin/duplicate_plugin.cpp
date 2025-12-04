/**
 * duplicate_plugin.cpp - Plugin with duplicate command names.
 *
 * This plugin intentionally uses the same command name "example" as the
 * example plugin to test duplicate command name handling.
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* This deliberately uses the same name as example_plugin */
static int example_command(void) {
    printf("Hello from the DUPLICATE plugin (this should NOT run if example was registered first)!\n");
    return 99;
}

/* A unique command from this plugin */
static int duplicate_unique(void) {
    printf("Duplicate plugin: unique command executed\n");
    return 100;
}

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    { "example", example_command },      /* Duplicate of example_plugin's "example" */
    { "duplicate_unique", duplicate_unique }
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "bu-duplicate-plugin",  /* plugin_name */
    1,                      /* version */
    2,                      /* cmd_count */
    s_commands              /* commands */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
