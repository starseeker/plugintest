/**
 * string_ops_plugin.cpp - Plugin with string operation commands.
 *
 * This plugin demonstrates custom command signatures for string operations:
 *   int (*)(int argc, const char** argv)
 */

#include <cstdio>
#include <cstring>

/* Define custom command signature BEFORE including bu_plugin.h */
#define BU_PLUGIN_CMD_RET int
#define BU_PLUGIN_CMD_ARGS int argc, const char** argv

/* When building a plugin, we export symbols */
#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif

#include "bu_plugin.h"

/* Command that reverses each argument */
static int cmd_reverse(int argc, const char** argv) {
    printf("reverse:");
    for (int i = 0; i < argc; i++) {
        size_t len = strlen(argv[i]);
        printf(" ");
        for (size_t j = len; j > 0; j--) {
            printf("%c", argv[i][j-1]);
        }
    }
    printf("\n");
    return argc;
}

/* Command that converts to uppercase */
static int cmd_upper(int argc, const char** argv) {
    printf("upper:");
    for (int i = 0; i < argc; i++) {
        printf(" ");
        for (const char* p = argv[i]; *p; p++) {
            char c = *p;
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
            printf("%c", c);
        }
    }
    printf("\n");
    return argc;
}

/* Command that counts total characters */
static int cmd_length(int argc, const char** argv) {
    size_t total = 0;
    for (int i = 0; i < argc; i++) {
        total += strlen(argv[i]);
    }
    printf("length: %zu total characters\n", total);
    return static_cast<int>(total);
}

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    { "reverse", cmd_reverse },
    { "upper", cmd_upper },
    { "length", cmd_length }
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "alt-string-ops-plugin",    /* plugin_name */
    1,                          /* version */
    3,                          /* cmd_count */
    s_commands,                 /* commands */
    BU_PLUGIN_ABI_VERSION,      /* abi_version */
    sizeof(bu_plugin_manifest)  /* struct_size */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
