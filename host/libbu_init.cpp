/**
 * libbu_init.cpp - Host library implementation.
 *
 * This file:
 *   - Defines BU_PLUGIN_IMPLEMENTATION to include the registry implementation
 *   - Includes the plugin core header which provides all API implementations
 *   - Registers built-in commands for testing
 *   - Provides bu_host_init() for application initialization
 */

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>

#ifndef BU_PLUGIN_IMPLEMENTATION
#define BU_PLUGIN_IMPLEMENTATION
#endif
#include "bu_plugin.h"

/*
 * The registry implementation is included via bu_plugin_core.h when
 * BU_PLUGIN_IMPLEMENTATION is defined.
 */

/* Storage for libexec directory used by path allow policy */
static std::string s_libexec_dir;

/**
 * Example logger callback that prefixes messages with the log level.
 * Applications can customize this for their logging framework.
 */
static void example_logger(int level, const char *msg) {
    const char *level_str = "INFO";
    switch (level) {
        case BU_LOG_INFO: level_str = "INFO"; break;
        case BU_LOG_WARN: level_str = "WARN"; break;
        case BU_LOG_ERR:  level_str = "ERROR"; break;
    }
    fprintf(stderr, "[bu_plugin] %s: %s\n", level_str, msg);
}

/**
 * Example path allow policy callback.
 * Only allows loading plugins from the configured libexec directory.
 * Returns 1 if allowed, 0 if denied.
 */
static int example_path_allow(const char *path) {
    if (!path || s_libexec_dir.empty()) {
        return 0;  /* Deny if no path or no libexec configured */
    }
    
    /* Check if the path starts with the libexec directory */
    size_t libexec_len = s_libexec_dir.length();
    if (std::strncmp(path, s_libexec_dir.c_str(), libexec_len) == 0) {
        /* Path must either be exactly libexec_dir or followed by a path separator */
        char next = path[libexec_len];
        if (next == '\0' || next == '/' || next == '\\') {
            return 1;  /* Allow */
        }
    }
    return 0;  /* Deny */
}

/**
 * bu_host_init - Initialize the host library with configuration.
 * @param libexec_dir  Directory where plugins are allowed to be loaded from.
 *                     If NULL or empty, path policy is not enforced.
 * @return 0 on success.
 *
 * This sets up:
 *   - The logger callback for centralized logging
 *   - The path allow policy to restrict plugin loading to approved directories
 *   - Initializes the plugin core
 */
extern "C" BU_PLUGIN_API int bu_host_init(const char *libexec_dir) {
    /* Set the example logger */
    bu_plugin_set_logger(example_logger);
    
    /* Configure path allow policy if libexec_dir is provided */
    if (libexec_dir && libexec_dir[0] != '\0') {
        s_libexec_dir = libexec_dir;
        bu_plugin_set_path_allow(example_path_allow);
    } else {
        s_libexec_dir.clear();
        bu_plugin_set_path_allow(nullptr);  /* Allow all paths */
    }
    
    /* Initialize the plugin core */
    return bu_plugin_init();
}

/* Built-in help command for testing */
static int builtin_help(void) {
    printf("Built-in help command\n");
    return 0;
}
REGISTER_BU_PLUGIN_COMMAND("help", builtin_help);

/* Built-in version command for testing */
static int builtin_version(void) {
    printf("Plugin Test Framework v1.0\n");
    return 1;
}
REGISTER_BU_PLUGIN_COMMAND("version", builtin_version);

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
REGISTER_BU_PLUGIN_COMMAND("status", builtin_status);
