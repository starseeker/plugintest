/**
 * bu_plugin.h - Wrapper configuring BU_PLUGIN_* for the bu plugin test host.
 *
 * This header sets up the plugin system for the test host with:
 *   - Command signature: int (*)(void)
 *   - Manifest symbol: bu_plugin_info
 */

#ifndef BU_PLUGIN_H
#define BU_PLUGIN_H

/*
 * The BU plugin core uses bu_plugin_cmd_impl for the function pointer type.
 * By default it's int (*)(void), which matches our test signature.
 * If a different signature were needed, we'd define it before including the core header.
 */

#include "bu_plugin_core.h"

/*
 * For convenience, we just use the bu_plugin_* types directly.
 * No additional aliases needed since we're using the bu_plugin naming throughout.
 */

#endif /* BU_PLUGIN_H */
