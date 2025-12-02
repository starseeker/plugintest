/**
 * special_names_plugin.cpp - Plugin with special/problematic command names.
 *
 * This plugin tests behavior with edge-case command names:
 *   - Very long names
 *   - Names with special characters
 *   - Empty string names
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "ged_plugin.h"

/* Command implementations */
static int cmd_long_name(void) {
    printf("Special names plugin: long name command executed\n");
    return 1;
}

static int cmd_underscore(void) {
    printf("Special names plugin: underscore command executed\n");
    return 2;
}

static int cmd_numbers(void) {
    printf("Special names plugin: numbers command executed\n");
    return 3;
}

static int cmd_mixed(void) {
    printf("Special names plugin: mixed case command executed\n");
    return 4;
}

/* Very long name to stress test string handling */
static const char* VERY_LONG_NAME = 
    "this_is_a_very_long_command_name_that_tests_buffer_handling_"
    "and_memory_allocation_for_extremely_long_identifiers_that_"
    "might_cause_issues_in_some_implementations";

/* Define the command array with various name patterns */
static bu_plugin_cmd s_commands[] = {
    { VERY_LONG_NAME, cmd_long_name },
    { "cmd__with__double__underscores", cmd_underscore },
    { "cmd_123_with_456_numbers_789", cmd_numbers },
    { "CamelCaseAndmixedCase", cmd_mixed },
    { "", cmd_mixed }  /* Empty string name - edge case */
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "ged-special-names-plugin", /* plugin_name */
    1,                          /* version */
    5,                          /* cmd_count */
    s_commands                  /* commands */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
