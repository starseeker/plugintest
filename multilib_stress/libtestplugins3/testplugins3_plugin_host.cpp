/**
 * testplugins3_plugin_host.cpp - Test Plugins 3 library host implementation.
 *
 * This file demonstrates an independent library (libtestplugins3) with its own plugin ecosystem.
 * The library uses BU_PLUGIN_NAME=testplugins3 to namespace its plugins.
 */

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>

/* Define the plugin namespace for this library */
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME testplugins3
#endif

#ifndef BU_PLUGIN_IMPLEMENTATION
#define BU_PLUGIN_IMPLEMENTATION
#endif
#include "bu_plugin.h"

/**
 * testplugins3_init - Initialize the testplugins3 library plugin system.
 * @return 0 on success.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins3_init(void) {
    bu_plugin_init();
    bu_plugin_logf(BU_LOG_INFO, "TestPlugins3 library plugin system initialized");
    return 0;
}

/**
 * testplugins3_shutdown - Shutdown the testplugins3 library plugin system.
 */
extern "C" BU_PLUGIN_EXPORT void testplugins3_shutdown(void) {
    bu_plugin_logf(BU_LOG_INFO, "TestPlugins3 library plugin system shutting down");
    bu_plugin_shutdown();
}

/**
 * testplugins3_load_plugin - Load a plugin into the testplugins3 library.
 * @param path Path to the plugin shared library.
 * @return Number of commands registered, or -1 on error.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins3_load_plugin(const char *path) {
    return bu_plugin_load(path);
}

/**
 * testplugins3_cmd_count - Get the number of registered testplugins3 commands.
 * @return Command count.
 */
extern "C" BU_PLUGIN_EXPORT size_t testplugins3_cmd_count(void) {
    return bu_plugin_cmd_count();
}

/**
 * testplugins3_cmd_exists - Check if a testplugins3 command exists.
 * @param name Command name.
 * @return 1 if exists, 0 otherwise.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins3_cmd_exists(const char *name) {
    return bu_plugin_cmd_exists(name);
}

/**
 * testplugins3_cmd_run - Execute a testplugins3 command by name.
 * @param name Command name.
 * @param result Output parameter for command result (can be NULL).
 * @return 0 on success, -1 if not found.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins3_cmd_run(const char *name, int *result) {
    return bu_plugin_cmd_run(name, result);
}

/* Built-in testplugins3 commands */
static int tp3_help(void) {
    printf("TestPlugins3: Built-in help command\n");
    return 0;
}
REGISTER_BU_PLUGIN_COMMAND("tp3_help", tp3_help);

static int tp3_version(void) {
    printf("TestPlugins3: Test Library v3.0\n");
    return 3;
}
REGISTER_BU_PLUGIN_COMMAND("tp3_version", tp3_version);

static int tp3_status(void) {
    size_t count = bu_plugin_cmd_count();
    printf("TestPlugins3: %zu commands registered\n", count);
    return static_cast<int>(count);
}
REGISTER_BU_PLUGIN_COMMAND("tp3_status", tp3_status);
