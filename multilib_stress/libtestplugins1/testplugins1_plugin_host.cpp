/**
 * testplugins1_plugin_host.cpp - Test Plugins 1 library host implementation.
 *
 * This file demonstrates an independent library (libtestplugins1) with its own plugin ecosystem.
 * The library uses BU_PLUGIN_NAME=testplugins1 to namespace its plugins.
 */

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>

/* Define the plugin namespace for this library */
#ifndef BU_PLUGIN_NAME
#define BU_PLUGIN_NAME testplugins1
#endif

#ifndef BU_PLUGIN_IMPLEMENTATION
#define BU_PLUGIN_IMPLEMENTATION
#endif
#include "bu_plugin.h"

/**
 * testplugins1_init - Initialize the testplugins1 library plugin system.
 * @return 0 on success.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins1_init(void) {
    bu_plugin_init();
    bu_plugin_logf(BU_LOG_INFO, "TestPlugins1 library plugin system initialized");
    return 0;
}

/**
 * testplugins1_shutdown - Shutdown the testplugins1 library plugin system.
 */
extern "C" BU_PLUGIN_EXPORT void testplugins1_shutdown(void) {
    bu_plugin_logf(BU_LOG_INFO, "TestPlugins1 library plugin system shutting down");
    bu_plugin_shutdown();
}

/**
 * testplugins1_load_plugin - Load a plugin into the testplugins1 library.
 * @param path Path to the plugin shared library.
 * @return Number of commands registered, or -1 on error.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins1_load_plugin(const char *path) {
    return bu_plugin_load(path);
}

/**
 * testplugins1_cmd_count - Get the number of registered testplugins1 commands.
 * @return Command count.
 */
extern "C" BU_PLUGIN_EXPORT size_t testplugins1_cmd_count(void) {
    return bu_plugin_cmd_count();
}

/**
 * testplugins1_cmd_exists - Check if a testplugins1 command exists.
 * @param name Command name.
 * @return 1 if exists, 0 otherwise.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins1_cmd_exists(const char *name) {
    return bu_plugin_cmd_exists(name);
}

/**
 * testplugins1_cmd_run - Execute a testplugins1 command by name.
 * @param name Command name.
 * @param result Output parameter for command result (can be NULL).
 * @return 0 on success, -1 if not found.
 */
extern "C" BU_PLUGIN_EXPORT int testplugins1_cmd_run(const char *name, int *result) {
    return bu_plugin_cmd_run(name, result);
}

/* Built-in testplugins1 commands */
static int tp1_help(void) {
    printf("TestPlugins1: Built-in help command\n");
    return 0;
}
REGISTER_BU_PLUGIN_COMMAND("tp1_help", tp1_help);

static int tp1_version(void) {
    printf("TestPlugins1: Test Library v1.0\n");
    return 1;
}
REGISTER_BU_PLUGIN_COMMAND("tp1_version", tp1_version);

static int tp1_status(void) {
    size_t count = bu_plugin_cmd_count();
    printf("TestPlugins1: %zu commands registered\n", count);
    return static_cast<int>(count);
}
REGISTER_BU_PLUGIN_COMMAND("tp1_status", tp1_status);
