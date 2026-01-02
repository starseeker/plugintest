/**
 * no_manifest_plugin.c - Test plugin that lacks bu_plugin_info symbol.
 *
 * This is a valid shared library, but it doesn't export the required
 * bu_plugin_info symbol. Used to test loader error reporting.
 */

#include <stdio.h>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif

/* Export a different symbol to make this a valid shared library */
#if defined(_WIN32) || defined(__CYGWIN__)
__declspec(dllexport) int some_other_function(void) {
    return 42;
}
#else
__attribute__((visibility("default"))) int some_other_function(void) {
    return 42;
}
#endif

/* Note: We intentionally do NOT export bu_plugin_info */
