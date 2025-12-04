/**
 * large_plugin.cpp - Plugin for testing scalability to hundreds of commands.
 *
 * This plugin:
 *   - Registers 500 commands to test scaling to BRL-CAD's needs
 *   - Uses macro generation for command definitions
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "bu_plugin.h"

/* Macro to generate command functions */
#define DEFINE_LARGE_CMD(n) \
    static int large_cmd_##n(void) { \
        return n; \
    }

/* Generate 500 commands using nested macros */
#define LARGE_10(base) \
    DEFINE_LARGE_CMD(base##0) \
    DEFINE_LARGE_CMD(base##1) \
    DEFINE_LARGE_CMD(base##2) \
    DEFINE_LARGE_CMD(base##3) \
    DEFINE_LARGE_CMD(base##4) \
    DEFINE_LARGE_CMD(base##5) \
    DEFINE_LARGE_CMD(base##6) \
    DEFINE_LARGE_CMD(base##7) \
    DEFINE_LARGE_CMD(base##8) \
    DEFINE_LARGE_CMD(base##9)

/* First 10 (0-9) */
DEFINE_LARGE_CMD(0)
DEFINE_LARGE_CMD(1)
DEFINE_LARGE_CMD(2)
DEFINE_LARGE_CMD(3)
DEFINE_LARGE_CMD(4)
DEFINE_LARGE_CMD(5)
DEFINE_LARGE_CMD(6)
DEFINE_LARGE_CMD(7)
DEFINE_LARGE_CMD(8)
DEFINE_LARGE_CMD(9)

/* 10-99 */
LARGE_10(1)
LARGE_10(2)
LARGE_10(3)
LARGE_10(4)
LARGE_10(5)
LARGE_10(6)
LARGE_10(7)
LARGE_10(8)
LARGE_10(9)

/* 100-199 */
LARGE_10(10)
LARGE_10(11)
LARGE_10(12)
LARGE_10(13)
LARGE_10(14)
LARGE_10(15)
LARGE_10(16)
LARGE_10(17)
LARGE_10(18)
LARGE_10(19)

/* 200-299 */
LARGE_10(20)
LARGE_10(21)
LARGE_10(22)
LARGE_10(23)
LARGE_10(24)
LARGE_10(25)
LARGE_10(26)
LARGE_10(27)
LARGE_10(28)
LARGE_10(29)

/* 300-399 */
LARGE_10(30)
LARGE_10(31)
LARGE_10(32)
LARGE_10(33)
LARGE_10(34)
LARGE_10(35)
LARGE_10(36)
LARGE_10(37)
LARGE_10(38)
LARGE_10(39)

/* 400-499 */
LARGE_10(40)
LARGE_10(41)
LARGE_10(42)
LARGE_10(43)
LARGE_10(44)
LARGE_10(45)
LARGE_10(46)
LARGE_10(47)
LARGE_10(48)
LARGE_10(49)

/* Macro to generate command entries */
#define CMD_ENTRY(n) { "large_" #n, large_cmd_##n }

#define CMD_10(base) \
    CMD_ENTRY(base##0), CMD_ENTRY(base##1), CMD_ENTRY(base##2), CMD_ENTRY(base##3), CMD_ENTRY(base##4), \
    CMD_ENTRY(base##5), CMD_ENTRY(base##6), CMD_ENTRY(base##7), CMD_ENTRY(base##8), CMD_ENTRY(base##9)

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    /* 0-9 */
    CMD_ENTRY(0), CMD_ENTRY(1), CMD_ENTRY(2), CMD_ENTRY(3), CMD_ENTRY(4),
    CMD_ENTRY(5), CMD_ENTRY(6), CMD_ENTRY(7), CMD_ENTRY(8), CMD_ENTRY(9),
    /* 10-99 */
    CMD_10(1), CMD_10(2), CMD_10(3), CMD_10(4), CMD_10(5),
    CMD_10(6), CMD_10(7), CMD_10(8), CMD_10(9),
    /* 100-199 */
    CMD_10(10), CMD_10(11), CMD_10(12), CMD_10(13), CMD_10(14),
    CMD_10(15), CMD_10(16), CMD_10(17), CMD_10(18), CMD_10(19),
    /* 200-299 */
    CMD_10(20), CMD_10(21), CMD_10(22), CMD_10(23), CMD_10(24),
    CMD_10(25), CMD_10(26), CMD_10(27), CMD_10(28), CMD_10(29),
    /* 300-399 */
    CMD_10(30), CMD_10(31), CMD_10(32), CMD_10(33), CMD_10(34),
    CMD_10(35), CMD_10(36), CMD_10(37), CMD_10(38), CMD_10(39),
    /* 400-499 */
    CMD_10(40), CMD_10(41), CMD_10(42), CMD_10(43), CMD_10(44),
    CMD_10(45), CMD_10(46), CMD_10(47), CMD_10(48), CMD_10(49)
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "bu-large-plugin",      /* plugin_name */
    1,                      /* version */
    500,                    /* cmd_count - 500 commands */
    s_commands              /* commands */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
