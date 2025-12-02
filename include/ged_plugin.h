/**
 * ged_plugin.h - Wrapper configuring BU_PLUGIN_* for the "ged" test host.
 *
 * This header sets up the plugin system for the test host with:
 *   - Command signature: int (*)(void)
 *   - Manifest symbol: ged_plugin_info
 */

#ifndef GED_PLUGIN_H
#define GED_PLUGIN_H

/*
 * The BU plugin core uses bu_plugin_cmd_impl for the function pointer type.
 * By default it's int (*)(void), which matches our test signature.
 * If a different signature were needed, we'd define it before including the core header.
 */

#include "bu_plugin_core.h"

/*
 * For convenience, we can alias the types to ged-specific names,
 * but for simplicity in this test we just use the bu_plugin_* types directly.
 */

/* GED-specific type aliases (optional, for clarity) */
typedef bu_plugin_cmd_impl ged_cmd_impl;
typedef bu_plugin_cmd ged_cmd;
typedef bu_plugin_manifest ged_manifest;

/*
 * Macro to declare a GED plugin manifest.
 * Identical to BU_PLUGIN_DECLARE_MANIFEST, provided for naming consistency.
 */
#define GED_PLUGIN_DECLARE_MANIFEST(manifest_var) BU_PLUGIN_DECLARE_MANIFEST(manifest_var)

/*
 * Macro to register a built-in GED command.
 * Identical to REGISTER_BU_PLUGIN_COMMAND, provided for naming consistency.
 */
#define REGISTER_GED_COMMAND(name, impl) REGISTER_BU_PLUGIN_COMMAND(name, impl)

#endif /* GED_PLUGIN_H */
