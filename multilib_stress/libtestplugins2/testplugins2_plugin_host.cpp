/**
 * testplugins2_plugin_host.cpp - Test Plugins 2 library host implementation.
 *
 * This file demonstrates an independent library (libtestplugins2) with its own plugin ecosystem.
 * The library uses BU_PLUGIN_NAME=testplugins2 to namespace its plugins.
 */

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>

/* Define the plugin namespace for this library */
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME testplugins2
#endif

#ifndef BU_PLUGIN_IMPLEMENTATION
#define BU_PLUGIN_IMPLEMENTATION
#endif
#include "bu_plugin.h"

/**
 * testplugins2_init - Initialize the testplugins2 library plugin system.
 * @return 0 on success.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins2_init(void) {
    bu_plugin_init();
    bu_plugin_logf(BU_LOG_INFO, "TestPlugins2 library plugin system initialized");
    return 0;
}

/**
 * testplugins2_shutdown - Shutdown the testplugins2 library plugin system.
 */
extern "C" BU_PLUGIN_EXPORT void testplugins2_shutdown(void) {
    bu_plugin_logf(BU_LOG_INFO, "TestPlugins2 library plugin system shutting down");
    bu_plugin_shutdown();
}

/**
 * testplugins2_load_plugin - Load a plugin into the testplugins2 library.
 * @param path Path to the plugin shared library.
 * @return Number of commands registered, or -1 on error.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins2_load_plugin(const char *path) {
    return bu_plugin_load(path);
}

/**
 * testplugins2_cmd_count - Get the number of registered testplugins2 commands.
 * @return Command count.
 */
extern "C" BU_PLUGIN_EXPORT size_t testplugins2_cmd_count(void) {
    return bu_plugin_cmd_count();
}

/**
 * testplugins2_cmd_exists - Check if a testplugins2 command exists.
 * @param name Command name.
 * @return 1 if exists, 0 otherwise.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins2_cmd_exists(const char *name) {
    return bu_plugin_cmd_exists(name);
}

/**
 * testplugins2_cmd_run - Execute a testplugins2 command by name.
 * @param name Command name.
 * @param result Output parameter for command result (can be NULL).
 * @return 0 on success, -1 if not found.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins2_cmd_run(const char *name, int *result) {
    return bu_plugin_cmd_run(name, result);
}

/* Built-in testplugins2 commands */
static int tp2_help(void) {
    printf("TestPlugins2: Built-in help command\n");
    return 0;
}
REGISTER_BU_PLUGIN_COMMAND("tp2_help", tp2_help);

static int tp2_version(void) {
    printf("TestPlugins2: Test Library v2.0\n");
    return 2;
}
REGISTER_BU_PLUGIN_COMMAND("tp2_version", tp2_version);

static int tp2_status(void) {
    size_t count = bu_plugin_cmd_count();
    printf("TestPlugins2: %zu commands registered\n", count);
    return static_cast<int>(count);
}
REGISTER_BU_PLUGIN_COMMAND("tp2_status", tp2_status);
