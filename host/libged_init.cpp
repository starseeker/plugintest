/**
 * libged_init.cpp - Host library implementation.
 *
 * This file:
 *   - Defines BU_PLUGIN_IMPLEMENTATION to include the registry implementation
 *   - Includes the plugin core header which provides all API implementations
 *   - Could register built-in commands here (none for this minimal test)
 */

#ifndef BU_PLUGIN_IMPLEMENTATION
#define BU_PLUGIN_IMPLEMENTATION
#endif
#include "ged_plugin.h"

/*
 * The registry implementation is included via bu_plugin_core.h when
 * BU_PLUGIN_IMPLEMENTATION is defined. No additional code needed here
 * for the basic functionality.
 *
 * If we wanted built-in commands, we could register them like this:
 *
 * static int builtin_help(void) {
 *     printf("Built-in help command\n");
 *     return 0;
 * }
 * REGISTER_GED_COMMAND("help", builtin_help);
 */
