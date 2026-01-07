/**
 * alt_sig_host.cpp - Host library for alternative signature plugin system.
 *
 * This file demonstrates using bu_plugin.h with a custom command signature:
 *   int (*)(int argc, const char** argv)
 *
 * Key points:
 *   - Defines BU_PLUGIN_CMD_RET and BU_PLUGIN_CMD_ARGS before including bu_plugin.h
 *   - Defines BU_PLUGIN_IMPLEMENTATION to include the registry implementation
 *   - Provides a custom wrapper function for running commands with arguments
 */

#include <cstdio>
#include <cstring>

/* Define custom command signature BEFORE including bu_plugin.h */
#define BU_PLUGIN_CMD_RET int
#define BU_PLUGIN_CMD_ARGS int argc, const char** argv

/* Enable the implementation in this compilation unit */
#ifndef BU_PLUGIN_IMPLEMENTATION
#define BU_PLUGIN_IMPLEMENTATION
#endif

#include "bu_plugin.h"

/**
 * alt_sig_host_init - Initialize the alternative signature plugin system.
 * @return 0 on success.
 */
extern "C" BU_PLUGIN_EXPORT int alt_sig_host_init(void) {
    return bu_plugin_init();
}

/**
 * alt_sig_cmd_run - Run a command with the alternative signature.
 * @param name     The command name to run.
 * @param argc     Number of arguments to pass to the command.
 * @param argv     Array of argument strings.
 * @param result   Output parameter for the command's return value (can be NULL).
 * @return 0 on success, -1 if command not found, -2 if command threw an exception.
 *
 * This is a custom wrapper tailored to the int (*)(int, const char**) signature.
 * It demonstrates how applications can provide their own run wrappers for
 * custom signatures.
 */
extern "C" BU_PLUGIN_EXPORT int alt_sig_cmd_run(const char *name, int argc, const char** argv, int *result) {
    bu_plugin_cmd_impl fn = bu_plugin_cmd_get(name);
    if (!fn) {
        bu_plugin_logf(BU_LOG_ERR, "Command '%s' not found", name ? name : "(null)");
        return -1;
    }

#ifdef __cplusplus
    try {
#endif
        int ret = fn(argc, argv);
        if (result) {
            *result = ret;
        }
        return 0;
#ifdef __cplusplus
    } catch (const std::exception& e) {
        bu_plugin_logf(BU_LOG_ERR, "Command '%s' threw exception: %s", name, e.what());
        return -2;
    } catch (...) {
        bu_plugin_logf(BU_LOG_ERR, "Command '%s' threw unknown exception", name);
        return -2;
    }
#endif
}

/* Built-in echo command for testing */
static int builtin_echo(int argc, const char** argv) {
    printf("echo:");
    for (int i = 0; i < argc; i++) {
        printf(" %s", argv[i]);
    }
    printf("\n");
    return argc;
}
REGISTER_BU_PLUGIN_COMMAND("echo", builtin_echo);

/* Built-in count command for testing */
static int builtin_count(int argc, const char** argv) {
    (void)argv; /* unused */
    printf("count: received %d arguments\n", argc);
    return argc;
}
REGISTER_BU_PLUGIN_COMMAND("count", builtin_count);
