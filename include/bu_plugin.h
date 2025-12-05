/**
 * bu_plugin.h - Wrapper configuring BU_PLUGIN_* for the bu plugin test host.
 *
 * This header sets up the plugin system for the test host with:
 *   - Command signature: int (*)(void) by default
 *   - Manifest symbol: bu_plugin_info
 *
 * Applications can define BU_PLUGIN_CMD_RET and/or BU_PLUGIN_CMD_ARGS before
 * including this header to customize the command function signature.
 */

#ifndef BU_PLUGIN_H
#define BU_PLUGIN_H

/*
 * The BU plugin core uses bu_plugin_cmd_impl for the function pointer type.
 * By default it's int (*)(void), which matches our test signature.
 * If a different signature were needed, define BU_PLUGIN_CMD_RET and/or
 * BU_PLUGIN_CMD_ARGS before including this header.
 *
 * Example for a different signature:
 *   #define BU_PLUGIN_CMD_RET int
 *   #define BU_PLUGIN_CMD_ARGS int argc, char** argv
 *   #include "bu_plugin.h"
 */

#include "bu_plugin_core.h"

/*
 * For convenience, we just use the bu_plugin_* types directly.
 * No additional aliases needed since we're using the bu_plugin naming throughout.
 */

#endif /* BU_PLUGIN_H */
