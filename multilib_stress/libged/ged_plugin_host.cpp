/**
 * ged_plugin_host.cpp - GED library host implementation.
 *
 * This file demonstrates an independent library (libged) with its own plugin ecosystem.
 * The library uses BU_PLUGIN_NAME=ged to namespace its plugins.
 */

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>

/* Define the plugin namespace for this library */
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME ged
#endif

#ifndef BU_PLUGIN_IMPLEMENTATION
#define BU_PLUGIN_IMPLEMENTATION
#endif
#include "bu_plugin.h"

/**
 * ged_init - Initialize the GED library plugin system.
 * @return 0 on success.
 */
extern "C" BU_PLUGIN_EXPORT int ged_init(void) {
    bu_plugin_init();
    bu_plugin_logf(BU_LOG_INFO, "GED library plugin system initialized");
    return 0;
}

/**
 * ged_shutdown - Shutdown the GED library plugin system.
 */
extern "C" BU_PLUGIN_EXPORT void ged_shutdown(void) {
    bu_plugin_logf(BU_LOG_INFO, "GED library plugin system shutting down");
    bu_plugin_shutdown();
}

/**
 * ged_load_plugin - Load a plugin into the GED library.
 * @param path Path to the plugin shared library.
 * @return Number of commands registered, or -1 on error.
 */
extern "C" BU_PLUGIN_EXPORT int ged_load_plugin(const char *path) {
    return bu_plugin_load(path);
}

/**
 * ged_cmd_count - Get the number of registered GED commands.
 * @return Command count.
 */
extern "C" BU_PLUGIN_EXPORT size_t ged_cmd_count(void) {
    return bu_plugin_cmd_count();
}

/**
 * ged_cmd_exists - Check if a GED command exists.
 * @param name Command name.
 * @return 1 if exists, 0 otherwise.
 */
extern "C" BU_PLUGIN_EXPORT int ged_cmd_exists(const char *name) {
    return bu_plugin_cmd_exists(name);
}

/**
 * ged_cmd_run - Execute a GED command by name.
 * @param name Command name.
 * @param result Output parameter for command result (can be NULL).
 * @return 0 on success, -1 if not found.
 */
extern "C" BU_PLUGIN_EXPORT int ged_cmd_run(const char *name, int *result) {
    return bu_plugin_cmd_run(name, result);
}

/* Built-in GED commands */
static int ged_help(void) {
    printf("GED: Built-in help command\n");
    return 0;
}
REGISTER_BU_PLUGIN_COMMAND("ged_help", ged_help);

static int ged_version(void) {
    printf("GED: Geometry Editing v1.0\n");
    return 1;
}
REGISTER_BU_PLUGIN_COMMAND("ged_version", ged_version);

static int ged_status(void) {
    size_t count = bu_plugin_cmd_count();
    printf("GED: %zu commands registered\n", count);
    return static_cast<int>(count);
}
REGISTER_BU_PLUGIN_COMMAND("ged_status", ged_status);
