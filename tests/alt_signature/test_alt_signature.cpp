/**
 * test_alt_signature.cpp - Test non-default function signatures
 *
 * This test validates that the plugin system works with custom command signatures
 * beyond the default int (*)(void) signature.
 *
 * It tests a signature: int (*)(int, const char**)
 * This demonstrates the BU_PLUGIN_CMD_RET and BU_PLUGIN_CMD_ARGS macros work correctly.
 *
 * This test now fully integrates with bu_plugin.h:
 *   - Uses the actual bu_plugin.h implementation via alt_sig_host library
 *   - Loads dynamic plugins with the alternative signature
 *   - Tests built-in commands registered at static initialization
 *   - Uses custom wrapper function alt_sig_cmd_run() for command execution
 */

#include <cstdio>
#include <cstring>
#include <string>

/* Define custom command signature before including the plugin header */
#define BU_PLUGIN_CMD_RET int
#define BU_PLUGIN_CMD_ARGS int argc, const char** argv

#include "bu_plugin.h"

/* External functions from the host library */
extern "C" {
    int alt_sig_host_init(void);
    int alt_sig_cmd_run(const char *name, int argc, const char** argv, int *result);
}

/* Helper function to construct plugin path */
static std::string get_plugin_path(const char* plugin_name) {
    std::string path = "./tests/alt_signature/plugins/";
#if defined(_WIN32)
    path += plugin_name;
    path += ".dll";
#elif defined(__APPLE__)
    path += plugin_name;
    path += ".dylib";
#else
    path += plugin_name;
    path += ".so";
#endif
    return path;
}


int main(void) {
    printf("========================================\n");
    printf("  Alternative Signature Test\n");
    printf("========================================\n");
    printf("\nThis test validates full integration of the plugin system with\n");
    printf("custom command signatures beyond int (*)(void).\n");
    printf("\nTesting signature: int (*)(int argc, const char** argv)\n\n");
    
    /* Initialize the plugin system */
    int init_result = alt_sig_host_init();
    if (init_result != 0) {
        printf("FAIL: Host initialization failed with code %d\n", init_result);
        return 1;
    }
    printf("Plugin system initialized\n");
    
    /* Check built-in commands registered at static initialization */
    size_t initial_count = bu_plugin_cmd_count();
    printf("Initial command count (built-ins): %zu\n", initial_count);
    if (initial_count < 2) {
        printf("FAIL: Expected at least 2 built-in commands (echo, count)\n");
        return 1;
    }
    
    /* Test 1: Built-in echo command */
    printf("\n=== Test 1: Built-in 'echo' command ===\n");
    const char* echo_args[] = {"hello", "world", "from", "builtin"};
    int result1 = 0;
    int status1 = alt_sig_cmd_run("echo", 4, echo_args, &result1);
    if (status1 != 0) {
        printf("FAIL: echo command run failed with status %d\n", status1);
        return 1;
    }
    if (result1 != 4) {
        printf("FAIL: Expected return value 4, got %d\n", result1);
        return 1;
    }
    printf("PASS: echo returned %d\n", result1);
    
    /* Test 2: Built-in count command */
    printf("\n=== Test 2: Built-in 'count' command ===\n");
    const char* count_args[] = {"a", "b", "c"};
    int result2 = 0;
    int status2 = alt_sig_cmd_run("count", 3, count_args, &result2);
    if (status2 != 0) {
        printf("FAIL: count command run failed with status %d\n", status2);
        return 1;
    }
    if (result2 != 3) {
        printf("FAIL: Expected return value 3, got %d\n", result2);
        return 1;
    }
    printf("PASS: count returned %d\n", result2);
    
    /* Load args plugin */
    printf("\n=== Loading args plugin ===\n");
    std::string args_plugin_path = get_plugin_path("alt-args-plugin");
    printf("Loading: %s\n", args_plugin_path.c_str());
    int loaded1 = bu_plugin_load(args_plugin_path.c_str());
    if (loaded1 < 0) {
        printf("FAIL: Failed to load args plugin\n");
        return 1;
    }
    printf("Loaded %d command(s) from args plugin\n", loaded1);
    
    /* Verify command count increased */
    size_t count_after_args = bu_plugin_cmd_count();
    if (count_after_args <= initial_count) {
        printf("FAIL: Command count should have increased\n");
        return 1;
    }
    printf("Total commands after args plugin: %zu\n", count_after_args);
    
    /* Test 3: args_test command from plugin */
    printf("\n=== Test 3: 'args_test' command from plugin ===\n");
    const char* args1[] = {"hello", "world", "test"};
    int result3 = 0;
    int status3 = alt_sig_cmd_run("args_test", 3, args1, &result3);
    if (status3 != 0) {
        printf("FAIL: args_test command run failed with status %d\n", status3);
        return 1;
    }
    if (result3 != 3) {
        printf("FAIL: Expected return value 3, got %d\n", result3);
        return 1;
    }
    printf("PASS: args_test returned %d\n", result3);
    
    /* Test 4: sum command from plugin */
    printf("\n=== Test 4: 'sum' command from plugin ===\n");
    const char* args2[] = {"10", "20", "30", "5"};
    int result4 = 0;
    int status4 = alt_sig_cmd_run("sum", 4, args2, &result4);
    if (status4 != 0) {
        printf("FAIL: sum command run failed with status %d\n", status4);
        return 1;
    }
    if (result4 != 65) {
        printf("FAIL: Expected sum 65, got %d\n", result4);
        return 1;
    }
    printf("PASS: sum returned %d\n", result4);
    
    /* Test 5: concat command from plugin */
    printf("\n=== Test 5: 'concat' command from plugin ===\n");
    const char* args3[] = {"Hello", "World"};
    int result5 = 0;
    int status5 = alt_sig_cmd_run("concat", 2, args3, &result5);
    if (status5 != 0) {
        printf("FAIL: concat command run failed with status %d\n", status5);
        return 1;
    }
    if (result5 != 2) {
        printf("FAIL: Expected return value 2, got %d\n", result5);
        return 1;
    }
    printf("PASS: concat returned %d\n", result5);
    
    /* Load string operations plugin */
    printf("\n=== Loading string_ops plugin ===\n");
    std::string string_ops_path = get_plugin_path("alt-string-ops-plugin");
    printf("Loading: %s\n", string_ops_path.c_str());
    int loaded2 = bu_plugin_load(string_ops_path.c_str());
    if (loaded2 < 0) {
        printf("FAIL: Failed to load string_ops plugin\n");
        return 1;
    }
    printf("Loaded %d command(s) from string_ops plugin\n", loaded2);
    
    /* Verify command count increased again */
    size_t count_after_string = bu_plugin_cmd_count();
    if (count_after_string <= count_after_args) {
        printf("FAIL: Command count should have increased\n");
        return 1;
    }
    printf("Total commands after string_ops plugin: %zu\n", count_after_string);
    
    /* Test 6: reverse command from string_ops plugin */
    printf("\n=== Test 6: 'reverse' command from plugin ===\n");
    const char* args4[] = {"hello", "world"};
    int result6 = 0;
    int status6 = alt_sig_cmd_run("reverse", 2, args4, &result6);
    if (status6 != 0) {
        printf("FAIL: reverse command run failed with status %d\n", status6);
        return 1;
    }
    if (result6 != 2) {
        printf("FAIL: Expected return value 2, got %d\n", result6);
        return 1;
    }
    printf("PASS: reverse returned %d\n", result6);
    
    /* Test 7: upper command from string_ops plugin */
    printf("\n=== Test 7: 'upper' command from plugin ===\n");
    const char* args5[] = {"test", "case"};
    int result7 = 0;
    int status7 = alt_sig_cmd_run("upper", 2, args5, &result7);
    if (status7 != 0) {
        printf("FAIL: upper command run failed with status %d\n", status7);
        return 1;
    }
    if (result7 != 2) {
        printf("FAIL: Expected return value 2, got %d\n", result7);
        return 1;
    }
    printf("PASS: upper returned %d\n", result7);
    
    /* Test 8: length command from string_ops plugin */
    printf("\n=== Test 8: 'length' command from plugin ===\n");
    const char* args6[] = {"Hello", "World"};
    int result8 = 0;
    int status8 = alt_sig_cmd_run("length", 2, args6, &result8);
    if (status8 != 0) {
        printf("FAIL: length command run failed with status %d\n", status8);
        return 1;
    }
    if (result8 != 10) {
        printf("FAIL: Expected length 10, got %d\n", result8);
        return 1;
    }
    printf("PASS: length returned %d\n", result8);
    
    /* Test 9: Test using bu_plugin_cmd_get for direct invocation */
    printf("\n=== Test 9: Direct invocation via bu_plugin_cmd_get ===\n");
    bu_plugin_cmd_impl sum_fn = bu_plugin_cmd_get("sum");
    if (!sum_fn) {
        printf("FAIL: Could not get 'sum' command\n");
        return 1;
    }
    const char* direct_args[] = {"100", "200"};
    int result9 = sum_fn(2, direct_args);
    if (result9 != 300) {
        printf("FAIL: Expected sum 300, got %d\n", result9);
        return 1;
    }
    printf("PASS: Direct invocation returned %d\n", result9);
    
    /* Test 10: Test bu_plugin_cmd_exists */
    printf("\n=== Test 10: Command existence checks ===\n");
    if (!bu_plugin_cmd_exists("sum")) {
        printf("FAIL: 'sum' should exist\n");
        return 1;
    }
    if (bu_plugin_cmd_exists("nonexistent")) {
        printf("FAIL: 'nonexistent' should not exist\n");
        return 1;
    }
    printf("PASS: Command existence checks work correctly\n");
    
    /* Test 11: Test bu_plugin_cmd_foreach */
    printf("\n=== Test 11: Iterate over all commands ===\n");
    size_t final_count = bu_plugin_cmd_count();
    printf("Final command count: %zu\n", final_count);
    
    /* Callback to count commands */
    struct CountData {
        size_t count;
    } count_data = {0};
    
    auto count_callback = [](const char *name, bu_plugin_cmd_impl impl, void *user_data) -> int {
        (void)impl;
        CountData *data = static_cast<CountData*>(user_data);
        data->count++;
        /* Print first few commands to verify */
        if (data->count <= 5) {
            printf("  Command %zu: %s\n", data->count, name);
        }
        return 0;  /* continue iteration */
    };
    
    bu_plugin_cmd_foreach(count_callback, &count_data);
    
    if (count_data.count != final_count) {
        printf("FAIL: foreach counted %zu commands but cmd_count returned %zu\n", 
               count_data.count, final_count);
        return 1;
    }
    printf("PASS: Iterated over %zu commands successfully\n", count_data.count);
    
    /* Summary */
    printf("\n========================================\n");
    printf("    Test Summary\n");
    printf("========================================\n");
    printf("All tests passed!\n");
    printf("\n✓ Custom signature int (*)(int, const char**) works correctly\n");
    printf("✓ Built-in commands registered at static initialization\n");
    printf("✓ Dynamic plugin loading with custom signatures\n");
    printf("✓ Custom wrapper function alt_sig_cmd_run() works\n");
    printf("✓ Direct command invocation via bu_plugin_cmd_get()\n");
    printf("✓ All bu_plugin.h API functions work with custom signatures\n");
    printf("✓ Successfully loaded and executed commands from %d plugins\n", loaded1 + loaded2);
    printf("✓ Total commands registered: %zu\n", final_count);
    printf("\nDemonstrates complete integration of bu_plugin.h with\n");
    printf("alternative function signatures through:\n");
    printf("  - BU_PLUGIN_CMD_RET and BU_PLUGIN_CMD_ARGS macros\n");
    printf("  - Custom host library with BU_PLUGIN_IMPLEMENTATION\n");
    printf("  - Application-specific wrapper functions for command execution\n");
    printf("========================================\n");
    
    return 0;
}
