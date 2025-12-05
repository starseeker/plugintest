/**
 * string_plugin.cpp - String operations plugin.
 *
 * This plugin:
 *   - Implements string-related commands
 *   - Tests coexistence with other plugins
 */

#include <cstdio>
#include <cstring>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* String command implementations */
static int string_length(void) {
    const char *test = "Hello, Plugin World!";
    printf("String plugin: length of '%s' is %zu\n", test, strlen(test));
    return static_cast<int>(strlen(test));
}

static int string_upper(void) {
    printf("String plugin: upper command executed\n");
    return 0;
}

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    { "string_length", string_length },
    { "string_upper", string_upper }
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "bu-string-plugin",     /* plugin_name */
    1,                      /* version */
    2,                      /* cmd_count */
    s_commands,             /* commands */
    BU_PLUGIN_ABI_VERSION,  /* abi_version */
    sizeof(bu_plugin_manifest) /* struct_size */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
