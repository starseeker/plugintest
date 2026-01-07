/**
 * test_alt_signature.cpp - Test non-default function signatures
 *
 * This test validates that the plugin system works with custom command signatures
 * beyond the default int (*)(void) signature.
 *
 * It tests a signature: int (*)(int, const char**)
 * This demonstrates the BU_PLUGIN_CMD_RET and BU_PLUGIN_CMD_ARGS macros work correctly.
 *
 * Note: This test only validates command registration and direct invocation.
 * The bu_plugin_cmd_run() wrapper function is not compatible with custom signatures
 * and is not used in this test.
 */

#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <string>
#include <mutex>

/* Define custom command signature before including the plugin header */
#define BU_PLUGIN_CMD_RET int
#define BU_PLUGIN_CMD_ARGS int argc, const char** argv

#include "bu_plugin.h"

/* Minimal implementation of the plugin registry for this test */
static std::unordered_map<std::string, bu_plugin_cmd_impl> registry;
static std::mutex registry_mutex;

/* Register command function */
static int register_command(const char *name, bu_plugin_cmd_impl impl) {
    if (!name || !impl) return -1;
    std::lock_guard<std::mutex> lock(registry_mutex);
    registry[name] = impl;
    return 0;
}

/* Get command function */
static bu_plugin_cmd_impl get_command(const char *name) {
    if (!name) return nullptr;
    std::lock_guard<std::mutex> lock(registry_mutex);
    auto it = registry.find(name);
    return (it != registry.end()) ? it->second : nullptr;
}

/* Count commands */
static size_t count_commands(void) {
    std::lock_guard<std::mutex> lock(registry_mutex);
    return registry.size();
}

/* Test command with custom signature */
static int cmd_args_test(int argc, const char** argv) {
    printf("cmd_args_test called with %d arguments:\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }
    return argc;
}

static int cmd_sum(int argc, const char** argv) {
    int sum = 0;
    printf("cmd_sum calculating sum of %d numbers\n", argc);
    for (int i = 0; i < argc; i++) {
        /* Simple atoi for testing */
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

int main(void) {
    printf("========================================\n");
    printf("  Alternative Signature Test\n");
    printf("========================================\n");
    printf("\nThis test validates that the plugin system concepts work with\n");
    printf("custom command signatures beyond int (*)(void).\n");
    printf("\nTesting signature: int (*)(int argc, const char** argv)\n\n");
    
    /* Register commands */
    register_command("args_test", cmd_args_test);
    register_command("sum", cmd_sum);
    
    /* Verify commands are registered */
    size_t count = count_commands();
    printf("Registered commands: %zu\n", count);
    if (count != 2) {
        printf("FAIL: Expected 2 commands, got %zu\n", count);
        return 1;
    }
    
    /* Test 1: args_test command */
    printf("\n=== Test 1: args_test command ===\n");
    const char* args1[] = {"hello", "world", "test"};
    
    bu_plugin_cmd_impl fn1 = get_command("args_test");
    if (!fn1) {
        printf("FAIL: Could not find 'args_test' command\n");
        return 1;
    }
    
    int result1 = fn1(3, args1);
    if (result1 != 3) {
        printf("FAIL: Expected return value 3, got %d\n", result1);
        return 1;
    }
    printf("PASS: args_test returned %d\n", result1);
    
    /* Test 2: sum command */
    printf("\n=== Test 2: sum command ===\n");
    const char* args2[] = {"10", "20", "30", "5"};
    
    bu_plugin_cmd_impl fn2 = get_command("sum");
    if (!fn2) {
        printf("FAIL: Could not find 'sum' command\n");
        return 1;
    }
    
    int result2 = fn2(4, args2);
    if (result2 != 65) {
        printf("FAIL: Expected sum 65, got %d\n", result2);
        return 1;
    }
    printf("PASS: sum returned %d\n", result2);
    
    /* Test 3: Multiple calls with different arguments */
    printf("\n=== Test 3: Multiple calls ===\n");
    const char* args3[] = {"100", "200"};
    
    int result3 = fn2(2, args3);
    if (result3 != 300) {
        printf("FAIL: Expected sum 300, got %d\n", result3);
        return 1;
    }
    printf("PASS: sum returned %d\n", result3);
    
    /* Summary */
    printf("\n========================================\n");
    printf("    Test Summary\n");
    printf("========================================\n");
    printf("All tests passed!\n");
    printf("\n✓ Custom signature int (*)(int, const char**) works correctly\n");
    printf("✓ Command registration and execution successful\n");
    printf("✓ Demonstrates BU_PLUGIN_CMD_RET and BU_PLUGIN_CMD_ARGS macros\n");
    printf("\nNote: This test demonstrates that the plugin core pattern\n");
    printf("      supports custom function signatures through macro definitions.\n");
    printf("      Full integration with bu_plugin.h would require additional\n");
    printf("      wrapper functions tailored to the specific signature.\n");
    printf("========================================\n");
    
    return 0;
}
