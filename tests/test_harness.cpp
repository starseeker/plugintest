/**
 * test_harness.cpp - Comprehensive test harness for the plugin system.
 *
 * This test harness:
 *   - Tests loading multiple plugins
 *   - Tests duplicate command name handling
 *   - Tests edge cases (null pointers, empty manifests, etc.)
 *   - Tests stress scenarios (many commands)
 *   - Tests scalability to hundreds of commands
 *   - Tests built-in commands alongside plugin commands
 *   - Tests command lookup and execution
 *   - Tests "first wins" precedence for duplicate names
 *   - Reports pass/fail status for each test
 *   - Provides performance benchmarks for lookup operations
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <chrono>
#include "bu_plugin.h"

/* Test statistics */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* Build configuration (for multi-config generators like Visual Studio) */
static std::string g_build_config;

/* Test assertion macros */
#define TEST_START(name) \
    do { \
        printf("\n=== TEST: %s ===\n", name); \
        tests_run++; \
    } while(0)

#define TEST_ASSERT(condition, msg) \
    do { \
        if (!(condition)) { \
            printf("  FAIL: %s\n", msg); \
            tests_failed++; \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL(expected, actual, msg) \
    do { \
        if ((expected) != (actual)) { \
            printf("  FAIL: %s (expected %d, got %d)\n", msg, static_cast<int>(expected), static_cast<int>(actual)); \
            tests_failed++; \
            return false; \
        } \
    } while(0)

#define TEST_PASS() \
    do { \
        printf("  PASS\n"); \
        tests_passed++; \
        return true; \
    } while(0)

/* Construct plugin path based on OS and build configuration */
static std::string get_plugin_path(const char* base_dir, const char* plugin_subdir, const char* plugin_name) {
    std::string path = base_dir;
    path += "/";
    path += plugin_subdir;
    path += "/";
    
#if defined(_WIN32) && defined(_MSC_VER)
    /* For MSVC multi-config builds, add the configuration subdirectory */
    if (!g_build_config.empty()) {
        path += g_build_config;
        path += "/";
    }
    /* MSVC doesn't use 'lib' prefix */
    path += plugin_name;
    path += ".dll";
#elif defined(_WIN32)
    /* For other Windows compilers (MinGW, etc.) */
    path += "lib";
    path += plugin_name;
    path += ".dll";
#elif defined(__APPLE__)
    path += "lib";
    path += plugin_name;
    path += ".dylib";
#else
    path += "lib";
    path += plugin_name;
    path += ".so";
#endif
    return path;
}

/* Test: Initial state */
static bool test_initial_state() {
    TEST_START("Initial State");
    
    size_t count = bu_plugin_cmd_count();
    printf("  Initial command count: %zu\n", count);
    
    /* Check that non-existent command returns 0 */
    int exists = bu_plugin_cmd_exists("nonexistent_command");
    TEST_ASSERT(exists == 0, "Non-existent command should return 0 for exists");
    
    /* Check that get returns NULL for non-existent command */
    bu_plugin_cmd_impl impl = bu_plugin_cmd_get("nonexistent_command");
    TEST_ASSERT(impl == nullptr, "Non-existent command should return NULL for get");
    
    TEST_PASS();
}

/* Test: Load single plugin */
static bool test_load_single_plugin(const char* plugin_dir) {
    TEST_START("Load Single Plugin");
    
    size_t count_before = bu_plugin_cmd_count();
    std::string path = get_plugin_path(plugin_dir, "plugin/example", "bu-example-plugin");
    
    printf("  Loading plugin: %s\n", path.c_str());
    int result = bu_plugin_load(path.c_str());
    
    TEST_ASSERT(result >= 0, "Plugin load should succeed");
    printf("  Registered %d command(s)\n", result);
    
    size_t count_after = bu_plugin_cmd_count();
    TEST_ASSERT(count_after > count_before, "Command count should increase");
    
    /* Verify the command was registered */
    TEST_ASSERT(bu_plugin_cmd_exists("example") == 1, "Command 'example' should exist");
    
    /* Execute the command */
    bu_plugin_cmd_impl fn = bu_plugin_cmd_get("example");
    TEST_ASSERT(fn != nullptr, "Should be able to get 'example' command");
    
    int cmd_result = fn();
    TEST_ASSERT_EQUAL(42, cmd_result, "Example command should return 42");
    
    TEST_PASS();
}

/* Test: Load multiple plugins */
static bool test_load_multiple_plugins(const char* plugin_dir) {
    TEST_START("Load Multiple Plugins");
    
    /* Load math plugin */
    std::string math_path = get_plugin_path(plugin_dir, "plugin/math_plugin", "bu-math-plugin");
    printf("  Loading math plugin: %s\n", math_path.c_str());
    int math_result = bu_plugin_load(math_path.c_str());
    TEST_ASSERT(math_result >= 0, "Math plugin load should succeed");
    printf("  Registered %d command(s) from math plugin\n", math_result);
    
    /* Load string plugin */
    std::string string_path = get_plugin_path(plugin_dir, "plugin/string_plugin", "bu-string-plugin");
    printf("  Loading string plugin: %s\n", string_path.c_str());
    int string_result = bu_plugin_load(string_path.c_str());
    TEST_ASSERT(string_result >= 0, "String plugin load should succeed");
    printf("  Registered %d command(s) from string plugin\n", string_result);
    
    size_t count_after = bu_plugin_cmd_count();
    printf("  Total commands after loading: %zu\n", count_after);
    
    /* Verify math commands exist */
    TEST_ASSERT(bu_plugin_cmd_exists("math_add") == 1, "Command 'math_add' should exist");
    TEST_ASSERT(bu_plugin_cmd_exists("math_multiply") == 1, "Command 'math_multiply' should exist");
    TEST_ASSERT(bu_plugin_cmd_exists("math_square") == 1, "Command 'math_square' should exist");
    
    /* Verify string commands exist */
    TEST_ASSERT(bu_plugin_cmd_exists("string_length") == 1, "Command 'string_length' should exist");
    TEST_ASSERT(bu_plugin_cmd_exists("string_upper") == 1, "Command 'string_upper' should exist");
    
    /* Execute and verify return values */
    bu_plugin_cmd_impl add_fn = bu_plugin_cmd_get("math_add");
    TEST_ASSERT(add_fn != nullptr, "Should be able to get 'math_add' command");
    TEST_ASSERT_EQUAL(5, add_fn(), "math_add should return 5");
    
    bu_plugin_cmd_impl mul_fn = bu_plugin_cmd_get("math_multiply");
    TEST_ASSERT(mul_fn != nullptr, "Should be able to get 'math_multiply' command");
    TEST_ASSERT_EQUAL(6, mul_fn(), "math_multiply should return 6");
    
    TEST_PASS();
}

/* Test: Duplicate command names */
static bool test_duplicate_names(const char* plugin_dir) {
    TEST_START("Duplicate Command Names");
    
    /* The example plugin should already be loaded with "example" command */
    TEST_ASSERT(bu_plugin_cmd_exists("example") == 1, "Command 'example' should already exist");
    
    /* Get the original function pointer */
    bu_plugin_cmd_impl original_fn = bu_plugin_cmd_get("example");
    TEST_ASSERT(original_fn != nullptr, "Original 'example' command should exist");
    int original_result = original_fn();
    printf("  Original 'example' command returns: %d\n", original_result);
    
    /* Load duplicate plugin which also has "example" command */
    std::string dup_path = get_plugin_path(plugin_dir, "plugin/duplicate_plugin", "bu-duplicate-plugin");
    printf("  Loading duplicate plugin: %s\n", dup_path.c_str());
    int dup_result = bu_plugin_load(dup_path.c_str());
    
    /* The load should succeed but only register the non-duplicate command */
    TEST_ASSERT(dup_result >= 0, "Duplicate plugin load should succeed (partial registration)");
    printf("  Registered %d command(s) from duplicate plugin\n", dup_result);
    
    /* The "example" command should still point to the original */
    bu_plugin_cmd_impl after_fn = bu_plugin_cmd_get("example");
    TEST_ASSERT(after_fn != nullptr, "'example' command should still exist");
    int after_result = after_fn();
    printf("  After loading duplicate, 'example' command returns: %d\n", after_result);
    
    /* The original command should still be used */
    TEST_ASSERT_EQUAL(original_result, after_result, 
        "Original 'example' command should still be registered (duplicate rejected)");
    
    /* The unique command from duplicate plugin should be registered */
    TEST_ASSERT(bu_plugin_cmd_exists("duplicate_unique") == 1, 
        "Unique command 'duplicate_unique' should be registered");
    
    TEST_PASS();
}

/* Test: Empty manifest */
static bool test_empty_manifest(const char* plugin_dir) {
    TEST_START("Empty Manifest");
    
    size_t count_before = bu_plugin_cmd_count();
    
    std::string empty_path = get_plugin_path(plugin_dir, "plugin/edge_cases", "bu-empty-plugin");
    printf("  Loading empty plugin: %s\n", empty_path.c_str());
    int result = bu_plugin_load(empty_path.c_str());
    
    /* Should return 0 (no commands registered, but not an error) */
    TEST_ASSERT_EQUAL(0, result, "Empty plugin should return 0 commands registered");
    
    size_t count_after = bu_plugin_cmd_count();
    TEST_ASSERT_EQUAL(count_before, count_after, "Command count should not change");
    
    TEST_PASS();
}

/* Test: Null implementations in manifest */
static bool test_null_implementations(const char* plugin_dir) {
    TEST_START("Null Implementations");
    
    std::string null_path = get_plugin_path(plugin_dir, "plugin/edge_cases", "bu-null-impl-plugin");
    printf("  Loading null-impl plugin: %s\n", null_path.c_str());
    int result = bu_plugin_load(null_path.c_str());
    
    printf("  Registered %d command(s)\n", result);
    
    /* Only the valid command should be registered */
    TEST_ASSERT(result >= 0, "Load should not fail");
    TEST_ASSERT(bu_plugin_cmd_exists("null_valid") == 1, "Valid command should be registered");
    
    /* Commands with null name or null impl should not be registered */
    TEST_ASSERT(bu_plugin_cmd_exists("null_impl") == 0, "Null impl command should not be registered");
    TEST_ASSERT(bu_plugin_cmd_exists("null_both") == 0, "Null-both command should not be registered");
    
    /* Execute the valid command */
    bu_plugin_cmd_impl fn = bu_plugin_cmd_get("null_valid");
    TEST_ASSERT(fn != nullptr, "Should be able to get valid command");
    int cmd_result = fn();
    TEST_ASSERT_EQUAL(1, cmd_result, "Valid command should return 1");
    
    TEST_PASS();
}

/* Test: Special command names */
static bool test_special_names(const char* plugin_dir) {
    TEST_START("Special Command Names");
    
    std::string special_path = get_plugin_path(plugin_dir, "plugin/edge_cases", "bu-special-names-plugin");
    printf("  Loading special-names plugin: %s\n", special_path.c_str());
    int result = bu_plugin_load(special_path.c_str());
    
    printf("  Registered %d command(s)\n", result);
    TEST_ASSERT(result >= 0, "Load should succeed");
    
    /* Test very long name */
    const char* long_name = 
        "this_is_a_very_long_command_name_that_tests_buffer_handling_"
        "and_memory_allocation_for_extremely_long_identifiers_that_"
        "might_cause_issues_in_some_implementations";
    TEST_ASSERT(bu_plugin_cmd_exists(long_name) == 1, "Very long command name should work");
    
    /* Test other special names */
    TEST_ASSERT(bu_plugin_cmd_exists("cmd__with__double__underscores") == 1, 
        "Double underscores should work");
    TEST_ASSERT(bu_plugin_cmd_exists("cmd_123_with_456_numbers_789") == 1, 
        "Numbers in name should work");
    TEST_ASSERT(bu_plugin_cmd_exists("CamelCaseAndmixedCase") == 1, 
        "Mixed case should work");
    
    TEST_PASS();
}

/* Test: Stress test with many commands */
static bool test_stress(const char* plugin_dir) {
    TEST_START("Stress Test (50 commands)");
    
    std::string stress_path = get_plugin_path(plugin_dir, "plugin/stress_plugin", "bu-stress-plugin");
    printf("  Loading stress plugin: %s\n", stress_path.c_str());
    int result = bu_plugin_load(stress_path.c_str());
    
    TEST_ASSERT_EQUAL(50, result, "Stress plugin should register 50 commands");
    
    size_t count_after = bu_plugin_cmd_count();
    printf("  Total commands after stress test: %zu\n", count_after);
    
    /* Verify all stress commands exist and return correct values */
    for (int i = 0; i < 50; i++) {
        char cmd_name[32];
        snprintf(cmd_name, sizeof(cmd_name), "stress_%d", i);
        
        TEST_ASSERT(bu_plugin_cmd_exists(cmd_name) == 1, "Stress command should exist");
        
        bu_plugin_cmd_impl fn = bu_plugin_cmd_get(cmd_name);
        TEST_ASSERT(fn != nullptr, "Should be able to get stress command");
        
        int cmd_result = fn();
        TEST_ASSERT_EQUAL(i, cmd_result, "Stress command should return its index");
    }
    
    printf("  All 50 stress commands verified\n");
    
    TEST_PASS();
}

/* Test: Scalability test with 500 commands */
static bool test_scalability(const char* plugin_dir) {
    TEST_START("Scalability Test (500 commands)");
    
    size_t count_before = bu_plugin_cmd_count();
    
    std::string large_path = get_plugin_path(plugin_dir, "plugin/large_plugin", "bu-large-plugin");
    printf("  Loading large plugin: %s\n", large_path.c_str());
    
    /* Time the plugin load */
    auto load_start = std::chrono::high_resolution_clock::now();
    int result = bu_plugin_load(large_path.c_str());
    auto load_end = std::chrono::high_resolution_clock::now();
    auto load_duration = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start);
    
    TEST_ASSERT_EQUAL(500, result, "Large plugin should register 500 commands");
    printf("  Plugin load time: %lld microseconds\n", static_cast<long long>(load_duration.count()));
    
    size_t count_after = bu_plugin_cmd_count();
    printf("  Total commands after loading: %zu\n", count_after);
    TEST_ASSERT(count_after >= count_before + 500, "Command count should increase by at least 500");
    
    /* Benchmark lookup performance with many commands */
    const int lookup_iterations = 1000;
    auto lookup_start = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < lookup_iterations; iter++) {
        for (int i = 0; i < 500; i += 50) {  /* Sample every 50th command */
            char cmd_name[32];
            snprintf(cmd_name, sizeof(cmd_name), "large_%d", i);
            bu_plugin_cmd_get(cmd_name);
        }
    }
    auto lookup_end = std::chrono::high_resolution_clock::now();
    auto lookup_duration = std::chrono::duration_cast<std::chrono::microseconds>(lookup_end - lookup_start);
    printf("  Lookup benchmark: %d lookups in %lld microseconds\n", 
           lookup_iterations * 10, static_cast<long long>(lookup_duration.count()));
    
    /* Verify a sampling of commands */
    for (int i = 0; i < 500; i += 100) {
        char cmd_name[32];
        snprintf(cmd_name, sizeof(cmd_name), "large_%d", i);
        
        TEST_ASSERT(bu_plugin_cmd_exists(cmd_name) == 1, "Large command should exist");
        
        bu_plugin_cmd_impl fn = bu_plugin_cmd_get(cmd_name);
        TEST_ASSERT(fn != nullptr, "Should be able to get large command");
        
        int cmd_result = fn();
        TEST_ASSERT_EQUAL(i, cmd_result, "Large command should return its index");
    }
    
    printf("  Sampled 5 large commands verified (0, 100, 200, 300, 400)\n");
    
    TEST_PASS();
}

/* Test: Invalid plugin paths */
static bool test_invalid_paths() {
    TEST_START("Invalid Plugin Paths");
    
    /* Test non-existent file */
    int result = bu_plugin_load("/nonexistent/path/to/plugin.so");
    TEST_ASSERT(result < 0, "Non-existent plugin should fail to load");
    
    /* Test null path */
    result = bu_plugin_load(nullptr);
    TEST_ASSERT(result < 0, "Null path should fail to load");
    
    /* Test empty path */
    result = bu_plugin_load("");
    TEST_ASSERT(result < 0, "Empty path should fail to load");
    
    TEST_PASS();
}

/* Test: Registry API with null parameters */
static bool test_null_api_params() {
    TEST_START("Null API Parameters");
    
    /* Test null name for exists */
    int exists = bu_plugin_cmd_exists(nullptr);
    TEST_ASSERT_EQUAL(0, exists, "Null name for exists should return 0");
    
    /* Test null name for get */
    bu_plugin_cmd_impl impl = bu_plugin_cmd_get(nullptr);
    TEST_ASSERT(impl == nullptr, "Null name for get should return NULL");
    
    /* Test null parameters for register */
    int result = bu_plugin_cmd_register(nullptr, nullptr);
    TEST_ASSERT(result != 0, "Null params for register should fail");
    
    /* Test null impl for register */
    result = bu_plugin_cmd_register("test_null_impl", nullptr);
    TEST_ASSERT(result != 0, "Null impl for register should fail");
    
    /* Test empty string name for register */
    auto dummy_fn = []() -> int { return 0; };
    result = bu_plugin_cmd_register("", dummy_fn);
    TEST_ASSERT(result != 0, "Empty string name for register should fail");
    
    TEST_PASS();
}

/* Test: Built-in commands */
static bool test_builtin_commands() {
    TEST_START("Built-in Commands");
    
    /* Check that built-in commands are registered */
    TEST_ASSERT(bu_plugin_cmd_exists("help") == 1, "Built-in 'help' command should exist");
    TEST_ASSERT(bu_plugin_cmd_exists("version") == 1, "Built-in 'version' command should exist");
    TEST_ASSERT(bu_plugin_cmd_exists("status") == 1, "Built-in 'status' command should exist");
    
    /* Execute help command */
    bu_plugin_cmd_impl help_fn = bu_plugin_cmd_get("help");
    TEST_ASSERT(help_fn != nullptr, "Should be able to get 'help' command");
    int help_result = help_fn();
    TEST_ASSERT_EQUAL(0, help_result, "Help command should return 0");
    
    /* Execute version command */
    bu_plugin_cmd_impl version_fn = bu_plugin_cmd_get("version");
    TEST_ASSERT(version_fn != nullptr, "Should be able to get 'version' command");
    int version_result = version_fn();
    TEST_ASSERT_EQUAL(1, version_result, "Version command should return 1");
    
    /* Execute status command - returns command count */
    bu_plugin_cmd_impl status_fn = bu_plugin_cmd_get("status");
    TEST_ASSERT(status_fn != nullptr, "Should be able to get 'status' command");
    int status_result = status_fn();
    TEST_ASSERT(status_result >= 3, "Status command should return at least 3 (built-in count)");
    
    TEST_PASS();
}

/* Callback for counting commands via foreach */
static int count_callback(const char* /*name*/, bu_plugin_cmd_impl /*impl*/, void* user_data) {
    int* count = static_cast<int*>(user_data);
    (*count)++;
    return 0;  /* Continue iteration */
}

/* Callback for finding a specific command */
struct FindData {
    const char* target;
    bool found;
};

static int find_callback(const char* name, bu_plugin_cmd_impl /*impl*/, void* user_data) {
    FindData* data = static_cast<FindData*>(user_data);
    if (strcmp(name, data->target) == 0) {
        data->found = true;
        return 1;  /* Stop iteration */
    }
    return 0;  /* Continue */
}

/* Test: Command enumeration */
static bool test_command_enumeration() {
    TEST_START("Command Enumeration");
    
    /* Count commands via foreach */
    int count = 0;
    bu_plugin_cmd_foreach(count_callback, &count);
    
    /* Should match bu_plugin_cmd_count */
    size_t expected = bu_plugin_cmd_count();
    TEST_ASSERT_EQUAL(static_cast<int>(expected), count, 
        "foreach count should match bu_plugin_cmd_count");
    printf("  Enumerated %d commands via foreach\n", count);
    
    /* Test finding specific commands */
    FindData help_data = { "help", false };
    bu_plugin_cmd_foreach(find_callback, &help_data);
    TEST_ASSERT(help_data.found, "Should find 'help' command via enumeration");
    
    FindData nonexistent_data = { "this_command_does_not_exist_xyz", false };
    bu_plugin_cmd_foreach(find_callback, &nonexistent_data);
    TEST_ASSERT(!nonexistent_data.found, "Should not find nonexistent command");
    
    /* Test null callback handling */
    bu_plugin_cmd_foreach(nullptr, nullptr);  /* Should not crash */
    
    TEST_PASS();
}

/* Test: Duplicate registration of same command */
static bool test_duplicate_register() {
    TEST_START("Duplicate Registration");
    
    /* Try to register a command that already exists (built-in 'help') */
    bu_plugin_cmd_impl original_fn = bu_plugin_cmd_get("help");
    TEST_ASSERT(original_fn != nullptr, "Original 'help' command should exist");
    
    /* Create a dummy function for testing */
    auto dummy_fn = []() -> int { return 999; };
    
    /* Try to register with the same name - should fail */
    int result = bu_plugin_cmd_register("help", dummy_fn);
    TEST_ASSERT(result != 0, "Duplicate registration should fail");
    
    /* Verify the original function is still registered */
    bu_plugin_cmd_impl after_fn = bu_plugin_cmd_get("help");
    TEST_ASSERT(after_fn == original_fn, "Original 'help' command should still be registered");
    
    TEST_PASS();
}

/* Test: Multiple duplicate attempts */
static bool test_multiple_duplicates() {
    TEST_START("Multiple Duplicate Attempts");
    
    /* Register a new command */
    auto test_fn = []() -> int { return 123; };
    int result = bu_plugin_cmd_register("test_multi_dup", test_fn);
    TEST_ASSERT_EQUAL(0, result, "Initial registration should succeed");
    
    /* Try to register duplicates multiple times */
    for (int i = 0; i < 10; i++) {
        auto dup_fn = []() -> int { return 456; };
        result = bu_plugin_cmd_register("test_multi_dup", dup_fn);
        TEST_ASSERT(result != 0, "Duplicate registration should fail");
    }
    
    /* Verify the original is still there and works */
    bu_plugin_cmd_impl fn = bu_plugin_cmd_get("test_multi_dup");
    TEST_ASSERT(fn != nullptr, "Command should still exist");
    TEST_ASSERT_EQUAL(123, fn(), "Original function should still be registered");
    
    TEST_PASS();
}

/* Main test runner */
int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("    Plugin System Test Harness\n");
    printf("========================================\n");
    
    /* Get plugin directory from command line or use default */
    const char* plugin_dir = ".";
    if (argc > 1) {
        plugin_dir = argv[1];
    }
    printf("Plugin directory: %s\n", plugin_dir);
    
    /* Get build configuration from command line (for multi-config generators) */
    if (argc > 2) {
        g_build_config = argv[2];
        printf("Build configuration: %s\n", g_build_config.c_str());
    }
    
    /* Initialize plugin system */
    if (bu_plugin_init() != 0) {
        fprintf(stderr, "Failed to initialize plugin system\n");
        return 1;
    }
    
    /* Run all tests */
    test_initial_state();
    test_builtin_commands();
    test_command_enumeration();
    test_null_api_params();
    test_duplicate_register();
    test_multiple_duplicates();
    test_invalid_paths();
    test_load_single_plugin(plugin_dir);
    test_load_multiple_plugins(plugin_dir);
    test_duplicate_names(plugin_dir);
    test_empty_manifest(plugin_dir);
    test_null_implementations(plugin_dir);
    test_special_names(plugin_dir);
    test_stress(plugin_dir);
    test_scalability(plugin_dir);
    
    /* Print summary */
    printf("\n========================================\n");
    printf("    Test Summary\n");
    printf("========================================\n");
    printf("Tests run:    %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("========================================\n");
    
    /* Return 0 if all tests passed, 1 otherwise */
    return (tests_failed == 0) ? 0 : 1;
}
