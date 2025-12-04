/**
 * stress_plugin.cpp - Plugin for stress testing.
 *
 * This plugin:
 *   - Registers a large number of commands to stress test the registry
 *   - Tests scalability of the command registry
 */

#include <cstdio>

#ifndef BU_PLUGIN_BUILDING_DLL
#define BU_PLUGIN_BUILDING_DLL
#endif
#include "ged_plugin.h"

/* Macro to generate command functions */
#define DEFINE_STRESS_CMD(n) \
    static int stress_cmd_##n(void) { \
        return n; \
    }

/* Generate 50 stress test commands */
DEFINE_STRESS_CMD(0)
DEFINE_STRESS_CMD(1)
DEFINE_STRESS_CMD(2)
DEFINE_STRESS_CMD(3)
DEFINE_STRESS_CMD(4)
DEFINE_STRESS_CMD(5)
DEFINE_STRESS_CMD(6)
DEFINE_STRESS_CMD(7)
DEFINE_STRESS_CMD(8)
DEFINE_STRESS_CMD(9)
DEFINE_STRESS_CMD(10)
DEFINE_STRESS_CMD(11)
DEFINE_STRESS_CMD(12)
DEFINE_STRESS_CMD(13)
DEFINE_STRESS_CMD(14)
DEFINE_STRESS_CMD(15)
DEFINE_STRESS_CMD(16)
DEFINE_STRESS_CMD(17)
DEFINE_STRESS_CMD(18)
DEFINE_STRESS_CMD(19)
DEFINE_STRESS_CMD(20)
DEFINE_STRESS_CMD(21)
DEFINE_STRESS_CMD(22)
DEFINE_STRESS_CMD(23)
DEFINE_STRESS_CMD(24)
DEFINE_STRESS_CMD(25)
DEFINE_STRESS_CMD(26)
DEFINE_STRESS_CMD(27)
DEFINE_STRESS_CMD(28)
DEFINE_STRESS_CMD(29)
DEFINE_STRESS_CMD(30)
DEFINE_STRESS_CMD(31)
DEFINE_STRESS_CMD(32)
DEFINE_STRESS_CMD(33)
DEFINE_STRESS_CMD(34)
DEFINE_STRESS_CMD(35)
DEFINE_STRESS_CMD(36)
DEFINE_STRESS_CMD(37)
DEFINE_STRESS_CMD(38)
DEFINE_STRESS_CMD(39)
DEFINE_STRESS_CMD(40)
DEFINE_STRESS_CMD(41)
DEFINE_STRESS_CMD(42)
DEFINE_STRESS_CMD(43)
DEFINE_STRESS_CMD(44)
DEFINE_STRESS_CMD(45)
DEFINE_STRESS_CMD(46)
DEFINE_STRESS_CMD(47)
DEFINE_STRESS_CMD(48)
DEFINE_STRESS_CMD(49)

/* Define the command array */
static bu_plugin_cmd s_commands[] = {
    { "stress_0", stress_cmd_0 },
    { "stress_1", stress_cmd_1 },
    { "stress_2", stress_cmd_2 },
    { "stress_3", stress_cmd_3 },
    { "stress_4", stress_cmd_4 },
    { "stress_5", stress_cmd_5 },
    { "stress_6", stress_cmd_6 },
    { "stress_7", stress_cmd_7 },
    { "stress_8", stress_cmd_8 },
    { "stress_9", stress_cmd_9 },
    { "stress_10", stress_cmd_10 },
    { "stress_11", stress_cmd_11 },
    { "stress_12", stress_cmd_12 },
    { "stress_13", stress_cmd_13 },
    { "stress_14", stress_cmd_14 },
    { "stress_15", stress_cmd_15 },
    { "stress_16", stress_cmd_16 },
    { "stress_17", stress_cmd_17 },
    { "stress_18", stress_cmd_18 },
    { "stress_19", stress_cmd_19 },
    { "stress_20", stress_cmd_20 },
    { "stress_21", stress_cmd_21 },
    { "stress_22", stress_cmd_22 },
    { "stress_23", stress_cmd_23 },
    { "stress_24", stress_cmd_24 },
    { "stress_25", stress_cmd_25 },
    { "stress_26", stress_cmd_26 },
    { "stress_27", stress_cmd_27 },
    { "stress_28", stress_cmd_28 },
    { "stress_29", stress_cmd_29 },
    { "stress_30", stress_cmd_30 },
    { "stress_31", stress_cmd_31 },
    { "stress_32", stress_cmd_32 },
    { "stress_33", stress_cmd_33 },
    { "stress_34", stress_cmd_34 },
    { "stress_35", stress_cmd_35 },
    { "stress_36", stress_cmd_36 },
    { "stress_37", stress_cmd_37 },
    { "stress_38", stress_cmd_38 },
    { "stress_39", stress_cmd_39 },
    { "stress_40", stress_cmd_40 },
    { "stress_41", stress_cmd_41 },
    { "stress_42", stress_cmd_42 },
    { "stress_43", stress_cmd_43 },
    { "stress_44", stress_cmd_44 },
    { "stress_45", stress_cmd_45 },
    { "stress_46", stress_cmd_46 },
    { "stress_47", stress_cmd_47 },
    { "stress_48", stress_cmd_48 },
    { "stress_49", stress_cmd_49 }
};

/* Define the manifest */
static bu_plugin_manifest s_manifest = {
    "ged-stress-plugin",    /* plugin_name */
    1,                      /* version */
    50,                     /* cmd_count - 50 commands */
    s_commands              /* commands */
};

/* Export the manifest */
BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
