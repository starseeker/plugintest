/**
 * rt_plugin_host.cpp - RT (Ray Tracing) library host implementation.
 *
 * This file demonstrates an independent library (librt) with its own plugin ecosystem.
 * The library uses BU_PLUGIN_NAME=rt to namespace its plugins.
 */

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>

/* Define the plugin namespace for this library */
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME rt
#endif

#ifndef BU_PLUGIN_IMPLEMENTATION
#define BU_PLUGIN_IMPLEMENTATION
#endif
#include "bu_plugin.h"

/**
 * rt_init - Initialize the RT library plugin system.
 * @return 0 on success.
 */
extern "C" BU_PLUGIN_EXPORT int rt_init(void) {
    bu_plugin_init();
    bu_plugin_logf(BU_LOG_INFO, "RT library plugin system initialized");
    return 0;
}

/**
 * rt_shutdown - Shutdown the RT library plugin system.
 */
extern "C" BU_PLUGIN_EXPORT void rt_shutdown(void) {
    bu_plugin_logf(BU_LOG_INFO, "RT library plugin system shutting down");
    bu_plugin_shutdown();
}

/**
 * rt_load_plugin - Load a plugin into the RT library.
 * @param path Path to the plugin shared library.
 * @return Number of commands registered, or -1 on error.
 */
extern "C" BU_PLUGIN_EXPORT int rt_load_plugin(const char *path) {
    return bu_plugin_load(path);
}

/**
 * rt_cmd_count - Get the number of registered RT commands.
 * @return Command count.
 */
extern "C" BU_PLUGIN_EXPORT size_t rt_cmd_count(void) {
    return bu_plugin_cmd_count();
}

/**
 * rt_cmd_exists - Check if an RT command exists.
 * @param name Command name.
 * @return 1 if exists, 0 otherwise.
 */
extern "C" BU_PLUGIN_EXPORT int rt_cmd_exists(const char *name) {
    return bu_plugin_cmd_exists(name);
}

/**
 * rt_cmd_run - Execute an RT command by name.
 * @param name Command name.
 * @param result Output parameter for command result (can be NULL).
 * @return 0 on success, -1 if not found.
 */
extern "C" BU_PLUGIN_EXPORT int rt_cmd_run(const char *name, int *result) {
    return bu_plugin_cmd_run(name, result);
}

/* Built-in RT commands */
static int rt_help(void) {
    printf("RT: Built-in help command\n");
    return 0;
}
REGISTER_BU_PLUGIN_COMMAND("rt_help", rt_help);

static int rt_version(void) {
    printf("RT: Ray Tracing v2.0\n");
    return 2;
}
REGISTER_BU_PLUGIN_COMMAND("rt_version", rt_version);

static int rt_status(void) {
    size_t count = bu_plugin_cmd_count();
    printf("RT: %zu commands registered\n", count);
    return static_cast<int>(count);
}
REGISTER_BU_PLUGIN_COMMAND("rt_status", rt_status);
