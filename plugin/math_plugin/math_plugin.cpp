/**
 * math_plugin.cpp - Math operations plugin.
 *
 * This plugin:
 *   - Implements multiple math commands
 *   - Tests loading plugins with multiple commands
 *   - Tests that different plugins can coexist
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* Math command implementations */
static int math_add(void) {
    printf("Math plugin: add command (2+3=5)\n");
    return 5;
}

static int math_multiply(void) {
    printf("Math plugin: multiply command (2*3=6)\n");
    return 6;
}

static int math_square(void) {
    printf("Math plugin: square command (4^2=16)\n");
    return 16;
}

/* Define the command array with multiple commands */
static bu_plugin_cmd s_commands[] = {
    { "math_add", math_add },
    { "math_multiply", math_multiply },
    { "math_square", math_square }
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "bu-math-plugin",       /* plugin_name */
    1,                      /* version */
    3,                      /* cmd_count - 3 commands */
    s_commands              /* commands */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
