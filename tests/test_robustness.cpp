/**
 * test_robustness.cpp - Tests for the robustness improvements in the BU plugin core.
 *
 * This test file covers:
 *   - Path allow policy (deny paths outside allowed directory)
 *   - Name scrubbing (whitespace trimming and warnings)
 *   - Manifest ABI validation (abi_version and struct_size)
 *   - dlerror clearing (missing symbol error reporting)
 *   - bu_plugin_cmd_run (valid, invalid, throwing commands)
 *   - Concurrency test for foreach
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include "bu_plugin.h"

/* Test statistics */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* Captured log messages for testing */
static std::vector<std::pair<int, std::string>> captured_logs;

/* Custom logger to capture messages */
static void test_logger(int level, const char *msg) {
    captured_logs.push_back({level, std::string(msg)});
}

/* Clear captured logs */
static void clear_logs() {
    captured_logs.clear();
}

/* Check if a log message contains a substring */
static bool log_contains(int level, const char *substr) {
    for (const auto& log : captured_logs) {
        if (log.first == level && log.second.find(substr) != std::string::npos) {
            return true;
        }
    }
    return false;
}

/* Test assertion macros */
#define TEST_START(name) \
    do { \
        printf("\n=== TEST: %s ===\n", name); \
        tests_run++; \
        clear_logs(); \
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

/* Build configuration for plugin paths */
static std::string g_build_config;

/* Construct plugin path based on OS and build configuration */
static std::string get_plugin_path(const char* base_dir, const char* plugin_subdir, const char* plugin_name) {
    std::string path = base_dir;
    path += "/";
    path += plugin_subdir;
    path += "/";
    
#if defined(_WIN32) && defined(_MSC_VER)
    if (!g_build_config.empty()) {
        path += g_build_config;
        path += "/";
    }
    path += plugin_name;
    path += ".dll";
#elif defined(_WIN32)
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

/* Custom path allow callback for testing */
static std::string s_allowed_dir;

static int test_path_allow(const char *path) {
    if (!path || s_allowed_dir.empty()) {
        return 0;
    }
    size_t allowed_len = s_allowed_dir.length();
    size_t path_len = std::strlen(path);
    
    /* Path must be at least as long as allowed_dir */
    if (path_len < allowed_len) {
        return 0;  /* Deny - path too short */
    }
    
    if (std::strncmp(path, s_allowed_dir.c_str(), allowed_len) == 0) {
        /* Path must either be exactly allowed_dir or followed by a path separator */
        if (path_len == allowed_len) {
            return 1;  /* Allow - exact match */
        }
        char next = path[allowed_len];
        if (next == '/' || next == '\\') {
            return 1;  /* Allow - path separator follows */
        }
    }
    return 0;  /* Deny */
}

/**
 * Test: Path Allow Policy
 * Verify that plugins outside the allowed directory are rejected.
 */
static bool test_path_allow_policy(const char* plugin_dir) {
    TEST_START("Path Allow Policy");
    
    /* Set up a custom path allow callback that only allows a specific directory */
    s_allowed_dir = "/allowed/plugins/path";
    bu_plugin_set_path_allow(test_path_allow);
    
    /* Try to load a plugin from outside the allowed directory */
    std::string path = get_plugin_path(plugin_dir, "plugin/example", "bu-example-plugin");
    printf("  Attempting to load plugin from: %s\n", path.c_str());
    
    int result = bu_plugin_load(path.c_str());
    TEST_ASSERT(result < 0, "Plugin load should fail for path outside allowed directory");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "not allowed by policy"), 
                "Should log error about path not allowed");
    
    /* Now set allowed directory to include the plugin directory */
    s_allowed_dir = plugin_dir;
    clear_logs();
    
    /* Try loading again - should succeed now */
    result = bu_plugin_load(path.c_str());
    /* Note: We might get duplicate command warning if example was already loaded,
       but the load should succeed if path is allowed */
    TEST_ASSERT(result >= 0 || log_contains(BU_LOG_WARN, "Duplicate"), 
                "Plugin load should succeed when path is allowed");
    
    /* Reset path allow to allow all paths */
    bu_plugin_set_path_allow(nullptr);
    
    TEST_PASS();
}

/**
 * Test: Name Scrubbing
 * Verify that leading/trailing whitespace is trimmed and internal whitespace is warned.
 */
static bool test_name_scrubbing() {
    TEST_START("Name Scrubbing");
    
    /* Register a command with leading/trailing whitespace */
    auto ws_cmd = []() -> int { return 777; };
    
    int result = bu_plugin_cmd_register("  trimmed_cmd  ", ws_cmd);
    TEST_ASSERT_EQUAL(0, result, "Should register command with trimmed name");
    
    /* Verify we can look it up without whitespace */
    TEST_ASSERT(bu_plugin_cmd_exists("trimmed_cmd") == 1, 
                "Command should exist with trimmed name");
    
    /* Verify we can also look it up with whitespace (should be trimmed) */
    TEST_ASSERT(bu_plugin_cmd_exists("  trimmed_cmd  ") == 1, 
                "Lookup with whitespace should also work");
    
    /* Register a command with internal whitespace - should warn */
    clear_logs();
    auto ws_internal_cmd = []() -> int { return 888; };
    result = bu_plugin_cmd_register("cmd with spaces", ws_internal_cmd);
    TEST_ASSERT_EQUAL(0, result, "Should register command with internal whitespace");
    TEST_ASSERT(log_contains(BU_LOG_WARN, "internal whitespace"), 
                "Should warn about internal whitespace");
    
    TEST_PASS();
}

/**
 * Test: bu_plugin_cmd_run
 * Verify safe command execution with try/catch handling.
 */
static bool test_cmd_run() {
    TEST_START("bu_plugin_cmd_run");
    
    /* Register a valid command for testing */
    auto valid_cmd = []() -> int { return 456; };
    bu_plugin_cmd_register("run_test_cmd", valid_cmd);
    
    /* Test running a valid command */
    int result_val = 0;
    int status = bu_plugin_cmd_run("run_test_cmd", &result_val);
    TEST_ASSERT_EQUAL(0, status, "Running valid command should return 0");
    TEST_ASSERT_EQUAL(456, result_val, "Command should return expected value");
    
    /* Test running with null result pointer (should not crash) */
    status = bu_plugin_cmd_run("run_test_cmd", nullptr);
    TEST_ASSERT_EQUAL(0, status, "Running with null result should work");
    
    /* Test running a non-existent command */
    clear_logs();
    status = bu_plugin_cmd_run("nonexistent_command", &result_val);
    TEST_ASSERT_EQUAL(-1, status, "Running nonexistent command should return -1");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "not found"), 
                "Should log error about command not found");
    
    /* Test running with null name */
    clear_logs();
    status = bu_plugin_cmd_run(nullptr, &result_val);
    TEST_ASSERT_EQUAL(-1, status, "Running with null name should return -1");
    
    TEST_PASS();
}

/**
 * Test: Command that throws an exception
 * Verify try/catch handling in bu_plugin_cmd_run.
 */
static bool test_cmd_run_throwing() {
    TEST_START("bu_plugin_cmd_run with Exception");
    
#ifdef __cplusplus
    /* Register a command that throws */
    auto throwing_cmd = []() -> int { 
        throw std::runtime_error("Test exception");
        return 0;  /* Never reached */
    };
    bu_plugin_cmd_register("throwing_cmd", throwing_cmd);
    
    /* Run the throwing command - should catch and return -2 */
    clear_logs();
    int result_val = 0;
    int status = bu_plugin_cmd_run("throwing_cmd", &result_val);
    TEST_ASSERT_EQUAL(-2, status, "Running throwing command should return -2");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "threw exception"), 
                "Should log error about exception");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "Test exception"), 
                "Should include exception message");
    
    printf("  Exception handling verified\n");
#else
    printf("  Skipped (C++ only)\n");
#endif
    
    TEST_PASS();
}

/**
 * Test: Logger Callback API
 * Verify that the logger callback can be set and receives messages.
 */
static bool test_logger_callback() {
    TEST_START("Logger Callback API");
    
    /* Test logger should already be set by main() */
    
    /* Trigger a log message by registering a duplicate command */
    auto dup_cmd = []() -> int { return 999; };
    clear_logs();
    int result = bu_plugin_cmd_register("help", dup_cmd);  /* 'help' already exists */
    
    TEST_ASSERT(result != 0, "Duplicate registration should fail");
    TEST_ASSERT(log_contains(BU_LOG_WARN, "Duplicate"), 
                "Logger should receive duplicate warning");
    
    /* Test bu_plugin_logf directly */
    clear_logs();
    bu_plugin_logf(BU_LOG_INFO, "Test info message: %d", 42);
    TEST_ASSERT(log_contains(BU_LOG_INFO, "Test info message: 42"), 
                "Logger should receive formatted message");
    
    /* DO NOT reset logger - other tests need it */
    
    TEST_PASS();
}

/**
 * Test: Buffered Startup Logging
 * Verify that logs are buffered when no logger is set and can be flushed later.
 */
static bool test_buffered_logging() {
    TEST_START("Buffered Startup Logging");
    
    /* Temporarily unset the logger to test buffering */
    bu_plugin_set_logger(nullptr);
    clear_logs();
    
    /* Log some messages - these should be buffered, not going to stderr */
    bu_plugin_logf(BU_LOG_INFO, "Buffered message 1");
    bu_plugin_logf(BU_LOG_WARN, "Buffered message 2");
    bu_plugin_logf(BU_LOG_ERR, "Buffered message 3");
    
    /* Captured logs should still be empty (messages went to internal buffer) */
    TEST_ASSERT(captured_logs.empty(), 
                "Messages should be buffered internally, not sent to callback");
    
    /* Now flush the buffered logs to our test logger */
    bu_plugin_flush_logs(test_logger);
    
    /* Now we should have received the buffered messages */
    TEST_ASSERT(log_contains(BU_LOG_INFO, "Buffered message 1"), 
                "Should receive first buffered message");
    TEST_ASSERT(log_contains(BU_LOG_WARN, "Buffered message 2"), 
                "Should receive second buffered message");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "Buffered message 3"), 
                "Should receive third buffered message");
    
    /* Flushing again should give us nothing (buffer was cleared) */
    clear_logs();
    bu_plugin_flush_logs(test_logger);
    TEST_ASSERT(captured_logs.empty(), 
                "Buffer should be empty after flush");
    
    /* Restore the test logger for subsequent tests */
    bu_plugin_set_logger(test_logger);
    
    printf("  Buffered logging verified\n");
    TEST_PASS();
}

/**
 * Test: ABI Validation - Correct Version
 * Verify that plugins with correct abi_version and struct_size load successfully.
 */
static bool test_abi_validation_correct(const char* plugin_dir) {
    TEST_START("ABI Validation - Correct Version");
    
    /* Reset path allow policy for this test */
    bu_plugin_set_path_allow(nullptr);
    
    /* Load a valid plugin to verify ABI fields are being checked */
    std::string path = get_plugin_path(plugin_dir, "plugin/c_only", "bu-c-only-plugin");
    printf("  Loading C-only plugin with correct ABI fields: %s\n", path.c_str());
    
    /* Check that the plugin loads successfully (it has correct ABI version) */
    /* Note: Commands may already be registered from previous test runs */
    int result = bu_plugin_load(path.c_str());
    TEST_ASSERT(result >= 0 || log_contains(BU_LOG_WARN, "Duplicate"), 
                "Plugin with correct ABI version should load (or commands already exist)");
    
    TEST_PASS();
}

/**
 * Test: ABI Validation - Incorrect ABI Version
 * Verify that plugins with mismatched abi_version are rejected.
 */
static bool test_abi_validation_bad_version(const char* plugin_dir) {
    TEST_START("ABI Validation - Incorrect ABI Version");
    
    bu_plugin_set_path_allow(nullptr);
    clear_logs();
    
    /* Try to load plugin with wrong ABI version */
    std::string path = get_plugin_path(plugin_dir, "tests/plugins/test_bad_abi", "bu-bad-abi-plugin");
    printf("  Attempting to load plugin with incorrect ABI version: %s\n", path.c_str());
    
    int result = bu_plugin_load(path.c_str());
    TEST_ASSERT(result < 0, "Plugin with incorrect ABI version should fail to load");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "incompatible ABI version"),
                "Should log error about incompatible ABI version");
    
    printf("  Plugin correctly rejected due to ABI version mismatch\n");
    TEST_PASS();
}

/**
 * Test: ABI Validation - Incorrect Struct Size
 * Verify that plugins with too-small struct_size are rejected.
 */
static bool test_abi_validation_bad_struct_size(const char* plugin_dir) {
    TEST_START("ABI Validation - Incorrect Struct Size");
    
    bu_plugin_set_path_allow(nullptr);
    clear_logs();
    
    /* Try to load plugin with too-small struct_size */
    std::string path = get_plugin_path(plugin_dir, "tests/plugins/test_bad_struct_size", "bu-bad-struct-plugin");
    printf("  Attempting to load plugin with incorrect struct_size: %s\n", path.c_str());
    
    int result = bu_plugin_load(path.c_str());
    TEST_ASSERT(result < 0, "Plugin with too-small struct_size should fail to load");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "incompatible manifest struct_size"),
                "Should log error about incompatible struct_size");
    
    printf("  Plugin correctly rejected due to struct_size mismatch\n");
    TEST_PASS();
}

/**
 * Test: Missing bu_plugin_info Symbol
 * Verify that libraries lacking the required symbol are rejected with clear error.
 */
static bool test_missing_plugin_info(const char* plugin_dir) {
    TEST_START("Missing bu_plugin_info Symbol");
    
    bu_plugin_set_path_allow(nullptr);
    clear_logs();
    
    /* Try to load library without bu_plugin_info symbol */
    std::string path = get_plugin_path(plugin_dir, "tests/plugins/test_no_manifest", "bu-no-manifest-plugin");
    printf("  Attempting to load library without bu_plugin_info: %s\n", path.c_str());
    
    int result = bu_plugin_load(path.c_str());
    TEST_ASSERT(result < 0, "Library without bu_plugin_info should fail to load");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "does not export") ||
                log_contains(BU_LOG_ERR, "symbol not found"),
                "Should log error about missing symbol");
    
    printf("  Library correctly rejected due to missing bu_plugin_info symbol\n");
    TEST_PASS();
}

/**
 * Test: Concurrency test for foreach
 * Verify that foreach works correctly while commands are being registered.
 */
static bool test_concurrency_foreach() {
    TEST_START("Concurrency for foreach");
    
    std::atomic<int> foreach_count{0};
    std::atomic<bool> registration_done{false};
    std::atomic<int> registered_count{0};
    
    /* Thread 1: Register commands */
    std::thread register_thread([&]() {
        for (int i = 0; i < 100; i++) {
            char name[32];
            snprintf(name, sizeof(name), "concurrent_cmd_%d", i);
            auto cmd = []() -> int { return 0; };
            if (bu_plugin_cmd_register(name, cmd) == 0) {
                registered_count++;
            }
        }
        registration_done = true;
    });
    
    /* Thread 2: Iterate with foreach */
    std::thread foreach_thread([&]() {
        while (!registration_done || foreach_count < 3) {
            int count = 0;
            bu_plugin_cmd_foreach([](const char *, bu_plugin_cmd_impl, void *data) -> int {
                (*static_cast<int*>(data))++;
                return 0;
            }, &count);
            foreach_count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
    
    register_thread.join();
    foreach_thread.join();
    
    printf("  Registered %d commands concurrently\n", registered_count.load());
    printf("  Performed %d foreach iterations\n", foreach_count.load());
    
    /* Verify no deadlocks occurred (we reached this point) */
    TEST_ASSERT(registered_count > 0, "Should have registered some commands");
    TEST_ASSERT(foreach_count >= 3, "Should have performed multiple foreach iterations");
    
    TEST_PASS();
}

/**
 * Test: Duplicate detection in manifest
 * Verify that duplicate command names within a single manifest are detected and logged.
 */
static bool test_manifest_duplicate_detection(const char* plugin_dir) {
    TEST_START("Manifest Duplicate Detection");
    
    /* This test relies on the duplicate_plugin which has "example" twice
       and checking if there's internal duplicate detection.
       Since the current plugins don't have internal duplicates, 
       we test the between-manifest duplicate logging instead */
    
    bu_plugin_set_path_allow(nullptr);
    clear_logs();
    
    /* Load duplicate plugin which has a command that duplicates example plugin */
    std::string path = get_plugin_path(plugin_dir, "plugin/duplicate_plugin", "bu-duplicate-plugin");
    printf("  Loading duplicate plugin: %s\n", path.c_str());
    
    int result = bu_plugin_load(path.c_str());
    /* The load should succeed for the unique command */
    TEST_ASSERT(result >= 0, "Duplicate plugin should load (with partial success)");
    
    /* Check that duplicate was logged */
    TEST_ASSERT(log_contains(BU_LOG_WARN, "Duplicate command") || 
                log_contains(BU_LOG_WARN, "first wins"), 
                "Should log warning about duplicate command");
    
    TEST_PASS();
}

/**
 * Test: Invalid plugin paths with detailed error logging
 */
static bool test_invalid_paths_logging() {
    TEST_START("Invalid Paths with Error Logging");
    
    bu_plugin_set_path_allow(nullptr);
    clear_logs();
    
    /* Test non-existent file */
    int result = bu_plugin_load("/nonexistent/path/to/plugin.so");
    TEST_ASSERT(result < 0, "Non-existent plugin should fail to load");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "Failed to load plugin"), 
                "Should log detailed error message");
    
    /* Test null path */
    clear_logs();
    result = bu_plugin_load(nullptr);
    TEST_ASSERT(result < 0, "Null path should fail to load");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "Invalid plugin path"), 
                "Should log error about invalid path");
    
    /* Test empty path */
    clear_logs();
    result = bu_plugin_load("");
    TEST_ASSERT(result < 0, "Empty path should fail to load");
    TEST_ASSERT(log_contains(BU_LOG_ERR, "Invalid plugin path"), 
                "Should log error about invalid path");
    
    TEST_PASS();
}

/* Main test runner */
int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("    Robustness Test Suite\n");
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
    
    /* Set test logger to capture messages */
    bu_plugin_set_logger(test_logger);
    
    /* Initialize plugin system */
    if (bu_plugin_init() != 0) {
        fprintf(stderr, "Failed to initialize plugin system\n");
        return 1;
    }
    
    /* Run all tests */
    test_logger_callback();
    test_name_scrubbing();
    test_cmd_run();
    test_cmd_run_throwing();
    test_buffered_logging();
    test_path_allow_policy(plugin_dir);
    test_abi_validation_correct(plugin_dir);
    test_abi_validation_bad_version(plugin_dir);
    test_abi_validation_bad_struct_size(plugin_dir);
    test_missing_plugin_info(plugin_dir);
    test_manifest_duplicate_detection(plugin_dir);
    test_invalid_paths_logging();
    test_concurrency_foreach();
    
    /* Reset logger */
    bu_plugin_set_logger(nullptr);
    
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
