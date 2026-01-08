/**
 * args_plugin.cpp - Plugin with alternative signature commands.
 *
 * This plugin demonstrates custom command signatures:
 *   int (*)(int argc, const char** argv)
 */

#include <cstdio>

/* Define custom command signature BEFORE including bu_plugin.h */
#define BU_PLUGIN_CMD_RET int
#define BU_PLUGIN_CMD_ARGS int argc, const char** argv

/* When building a plugin, we export symbols */
#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif

#include "bu_plugin.h"

/* Command that prints all arguments */
static int cmd_args_test(int argc, const char** argv) {
    printf("cmd_args_test called with %d arguments:\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }
    return argc;
}

/* Command that sums numeric arguments */
static int cmd_sum(int argc, const char** argv) {
    int sum = 0;
    printf("cmd_sum calculating sum of %d numbers\n", argc);
    for (int i = 0; i < argc; i++) {
        /* Simple atoi for testing - only handles positive integers, ignores non-numeric chars */
        int val = 0;
        for (const char* p = argv[i]; *p; p++) {
            if (*p >= '0' && *p <= '9') {
                val = val * 10 + (*p - '0');
            }
        }
        printf("  argv[%d] = %s -> %d\n", i, argv[i], val);
        sum += val;
    }
    printf("  Sum = %d\n", sum);
    return sum;
}

/* Command that concatenates arguments */
static int cmd_concat(int argc, const char** argv) {
    printf("concat:");
    for (int i = 0; i < argc; i++) {
        printf("%s", argv[i]);
    }
    printf("\n");
    return argc;
}

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    { "args_test", cmd_args_test },
    { "sum", cmd_sum },
    { "concat", cmd_concat }
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "alt-args-plugin",          /* plugin_name */
    1,                          /* version */
    3,                          /* cmd_count */
    s_commands,                 /* commands */
    BU_PLUGIN_ABI_VERSION,      /* abi_version */
    sizeof(bu_plugin_manifest)  /* struct_size */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
