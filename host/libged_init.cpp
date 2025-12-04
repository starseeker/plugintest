/**
 * libged_init.cpp - Host library implementation.
 *
 * This file:
 *   - Defines BU_PLUGIN_IMPLEMENTATION to include the registry implementation
 *   - Includes the plugin core header which provides all API implementations
 *   - Registers built-in commands for testing
 */

#include <cstdio>
#include <climits>

#ifndef BU_PLUGIN_IMPLEMENTATION
#define BU_PLUGIN_IMPLEMENTATION
#endif
#include "ged_plugin.h"

/*
 * The registry implementation is included via bu_plugin_core.h when
 * BU_PLUGIN_IMPLEMENTATION is defined.
 */

/* Built-in help command for testing */
static int builtin_help(void) {
    printf("Built-in help command\n");
    return 0;
}
REGISTER_GED_COMMAND("help", builtin_help);

/* Built-in version command for testing */
static int builtin_version(void) {
    printf("Plugin Test Framework v1.0\n");
    return 1;
}
REGISTER_GED_COMMAND("version", builtin_version);

/* Built-in status command for testing */
static int builtin_status(void) {
    size_t count = bu_plugin_cmd_count();
    printf("Status: OK, %zu commands registered\n", count);
    /* Return count, clamped to INT_MAX if too large */
    if (count > static_cast<size_t>(INT_MAX)) {
        return INT_MAX;
    }
    return static_cast<int>(count);
}
REGISTER_GED_COMMAND("status", builtin_status);
