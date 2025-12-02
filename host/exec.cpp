/**
 * exec.cpp - Test runner executable.
 *
 * This executable:
 *   - Initializes the plugin registry
 *   - Optionally loads a plugin from a command-line path
 *   - Reports registry size
 *   - Runs the "example" command if registered
 */

#include <cstdio>
#include <cstdlib>
#include "ged_plugin.h"

int main(int argc, char *argv[]) {
    /* Initialize the plugin system */
    if (bu_plugin_init() != 0) {
        fprintf(stderr, "Failed to initialize plugin system\n");
        return 1;
    }

    /* Report initial state */
    printf("Initial registered count: %lu\n", (unsigned long)bu_plugin_cmd_count());

    /* If a plugin path is provided, load it */
    if (argc > 1) {
        const char *plugin_path = argv[1];
        int loaded = bu_plugin_load(plugin_path);
        if (loaded < 0) {
            fprintf(stderr, "Failed to load plugin: %s\n", plugin_path);
            return 1;
        }
        printf("Registered %d command(s) from %s\n", loaded, plugin_path);
    }

    /* Report final state */
    printf("Final registered count: %lu\n", (unsigned long)bu_plugin_cmd_count());

    /* Try to run the "example" command */
    if (bu_plugin_cmd_exists("example")) {
        printf("Running 'example' command...\n");
        bu_plugin_cmd_impl fn = bu_plugin_cmd_get("example");
        if (fn) {
            int result = fn();
            printf("Command 'example' returned: %d\n", result);
        }
    } else {
        printf("Command 'example' not registered.\n");
    }

    return 0;
}
