/**
 * multilib_stress_test.cpp - Comprehensive stress test for multiple independent libraries.
 *
 * This test:
 *   - Loads 3 independent libraries (TestPlugins1, TestPlugins2, TestPlugins3) each with their own plugin ecosystem
 *   - Each library uses the same bu_plugin_core.h but with different BU_PLUGIN_NAME namespace
 *   - Loads plugins for each library
 *   - Executes commands from each library's plugin ecosystem
 *   - Tests proper shutdown/unload ordering
 *   - Verifies no cross-library interference
 *
 * This constitutes the full stress test of the plugin system as it would be used in a
 * real application with multiple independent libraries.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
typedef HMODULE lib_handle_t;
#define LOAD_LIBRARY(path) LoadLibraryA(path)
#define GET_SYMBOL(handle, name) GetProcAddress(handle, name)
#define UNLOAD_LIBRARY(handle) FreeLibrary(handle)
#define LIB_EXT ".dll"
#define LIB_PREFIX ""
#else
#include <dlfcn.h>
typedef void* lib_handle_t;
#define LOAD_LIBRARY(path) dlopen(path, RTLD_NOW | RTLD_LOCAL)
#define GET_SYMBOL(handle, name) dlsym(handle, name)
#define UNLOAD_LIBRARY(handle) dlclose(handle)
#ifdef __APPLE__
#define LIB_EXT ".dylib"
#else
#define LIB_EXT ".so"
#endif
#define LIB_PREFIX "lib"
#endif

/* Define the command implementation function pointer type */
typedef int (*bu_plugin_cmd_impl)(void);

/* Build configuration (for multi-config generators like Visual Studio) */
static std::string g_build_config;

/* Test statistics */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

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
            printf("  FAIL: %s (expected %d, got %d)\n", msg, \
                   static_cast<int>(expected), static_cast<int>(actual)); \
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

/* Build plugin path based on OS */
static std::string get_plugin_path(const char* lib_name, const char* plugin_name) {
    std::string path = "./tests/multilib_stress/plugins/";
    path += lib_name;
    path += "/";
    
#if defined(_WIN32) && defined(_MSC_VER)
    /* For MSVC multi-config builds, add the configuration subdirectory */
    if (!g_build_config.empty()) {
        path += g_build_config;
        path += "/";
    }
#endif
    
    path += LIB_PREFIX;
    path += plugin_name;
    path += LIB_EXT;
    return path;
}

/* Build library path based on OS */
static std::string get_library_path(const char* lib_name) {
    std::string path = "./tests/multilib_stress/lib";
    path += lib_name;
    path += "/";
    
#if defined(_WIN32) && defined(_MSC_VER)
    /* For MSVC multi-config builds, add the configuration subdirectory */
    if (!g_build_config.empty()) {
        path += g_build_config;
        path += "/";
    }
#endif
    
    path += LIB_PREFIX;
    path += lib_name;
    path += "_plugin_host";
    path += LIB_EXT;
    return path;
}

/* Library handle and function pointers */
struct LibraryAPI {
    lib_handle_t handle;
    const char* name;
    int (*init)(void);
    void (*shutdown)(void);
    int (*load_plugin)(const char*);
    size_t (*cmd_count)(void);
    int (*cmd_exists)(const char*);
    int (*cmd_run)(const char*, int*);
    bu_plugin_cmd_impl (*cmd_get)(const char*);
};

/* Load a library and its API functions */
static bool load_library_api(LibraryAPI& api, const char* lib_name) {
    std::string path = get_library_path(lib_name);
    printf("  Loading library: %s\n", path.c_str());
    
    api.handle = LOAD_LIBRARY(path.c_str());
    if (!api.handle) {
        printf("    ERROR: Failed to load library\n");
        return false;
    }
    
    api.name = lib_name;
    
    /* Load function pointers */
    char fn_name[256];
    
    snprintf(fn_name, sizeof(fn_name), "%s_init", lib_name);
    api.init = reinterpret_cast<int(*)(void)>(GET_SYMBOL(api.handle, fn_name));
    
    snprintf(fn_name, sizeof(fn_name), "%s_shutdown", lib_name);
    api.shutdown = reinterpret_cast<void(*)(void)>(GET_SYMBOL(api.handle, fn_name));
    
    snprintf(fn_name, sizeof(fn_name), "%s_load_plugin", lib_name);
    api.load_plugin = reinterpret_cast<int(*)(const char*)>(GET_SYMBOL(api.handle, fn_name));
    
    snprintf(fn_name, sizeof(fn_name), "%s_cmd_count", lib_name);
    api.cmd_count = reinterpret_cast<size_t(*)(void)>(GET_SYMBOL(api.handle, fn_name));
    
    snprintf(fn_name, sizeof(fn_name), "%s_cmd_exists", lib_name);
    api.cmd_exists = reinterpret_cast<int(*)(const char*)>(GET_SYMBOL(api.handle, fn_name));
    
    snprintf(fn_name, sizeof(fn_name), "%s_cmd_run", lib_name);
    api.cmd_run = reinterpret_cast<int(*)(const char*, int*)>(GET_SYMBOL(api.handle, fn_name));
    
    snprintf(fn_name, sizeof(fn_name), "%s_cmd_get", lib_name);
    api.cmd_get = reinterpret_cast<bu_plugin_cmd_impl(*)(const char*)>(GET_SYMBOL(api.handle, fn_name));
    
    if (!api.init || !api.shutdown || !api.load_plugin || 
        !api.cmd_count || !api.cmd_exists || !api.cmd_run || !api.cmd_get) {
        printf("    ERROR: Failed to load all API functions\n");
        UNLOAD_LIBRARY(api.handle);
        return false;
    }
    
    printf("    ✓ Library loaded successfully\n");
    return true;
}

/* Test: Load all three libraries */
static bool test_load_libraries(std::vector<LibraryAPI>& libraries) {
    TEST_START("Load Multiple Independent Libraries");
    
    const char* lib_names[] = { "testplugins1", "testplugins2", "testplugins3" };
    
    for (size_t i = 0; i < 3; i++) {
        LibraryAPI api;
        if (!load_library_api(api, lib_names[i])) {
            TEST_ASSERT(false, "Failed to load library");
        }
        libraries.push_back(api);
    }
    
    printf("  ✓ All 3 libraries loaded successfully\n");
    TEST_PASS();
}

/* Test: Initialize all libraries */
static bool test_initialize_libraries(std::vector<LibraryAPI>& libraries) {
    TEST_START("Initialize All Libraries");
    
    for (auto& lib : libraries) {
        printf("  Initializing %s library...\n", lib.name);
        int result = lib.init();
        TEST_ASSERT(result == 0, "Library initialization should succeed");
        
        size_t count = lib.cmd_count();
        printf("    Initial command count: %zu\n", count);
        TEST_ASSERT(count == 5, "Should have 5 built-in commands (including colliding names)");
    }
    
    printf("  ✓ All libraries initialized with built-in commands\n");
    TEST_PASS();
}

/* Test: Load plugins for each library */
static bool test_load_plugins(std::vector<LibraryAPI>& libraries) {
    TEST_START("Load Plugins for Each Library");
    
    struct PluginInfo {
        const char* lib_name;
        const char* plugin_name;
        int expected_cmds;
    };
    
    PluginInfo plugins[] = {
        {"testplugins1", "tp1-draw-plugin", 3},
        {"testplugins1", "tp1-edit-plugin", 2},
        {"testplugins2", "tp2-shader-plugin", 3},
        {"testplugins2", "tp2-render-plugin", 2},
        {"testplugins3", "tp3-overlap-plugin", 3},
        {"testplugins3", "tp3-volume-plugin", 2}
    };
    
    for (size_t i = 0; i < 6; i++) {
        PluginInfo& pi = plugins[i];
        
        /* Find the library */
        LibraryAPI* lib = nullptr;
        for (auto& l : libraries) {
            if (strcmp(l.name, pi.lib_name) == 0) {
                lib = &l;
                break;
            }
        }
        TEST_ASSERT(lib != nullptr, "Library should exist");
        
        std::string path = get_plugin_path(pi.lib_name, pi.plugin_name);
        printf("  Loading %s plugin: %s\n", pi.lib_name, path.c_str());
        
        int result = lib->load_plugin(path.c_str());
        TEST_ASSERT(result >= 0, "Plugin load should succeed");
        TEST_ASSERT_EQUAL(pi.expected_cmds, result, "Should register expected number of commands");
        printf("    ✓ Registered %d command(s)\n", result);
    }
    
    /* Verify final command counts */
    printf("\n  Final command counts per library:\n");
    for (auto& lib : libraries) {
        size_t count = lib.cmd_count();
        printf("    %s: %zu commands\n", lib.name, count);
        if (strcmp(lib.name, "testplugins1") == 0) {
            TEST_ASSERT(count == 10, "TestPlugins1 should have 10 commands (5 built-in + 5 plugin)");
        } else if (strcmp(lib.name, "testplugins2") == 0) {
            TEST_ASSERT(count == 10, "TestPlugins2 should have 10 commands (5 built-in + 5 plugin)");
        } else if (strcmp(lib.name, "testplugins3") == 0) {
            TEST_ASSERT(count == 10, "TestPlugins3 should have 10 commands (5 built-in + 5 plugin)");
        }
    }
    
    printf("  ✓ All plugins loaded successfully\n");
    TEST_PASS();
}

/* Test: Execute commands from each library */
static bool test_execute_commands(std::vector<LibraryAPI>& libraries) {
    TEST_START("Execute Commands from Each Library");
    
    struct CommandTest {
        const char* lib_name;
        const char* cmd_name;
        int expected_result;
    };
    
    CommandTest tests[] = {
        /* TestPlugins1 commands */
        {"testplugins1", "tp1_help", 0},
        {"testplugins1", "tp1_version", 1},
        {"testplugins1", "tp1_draw", 100},
        {"testplugins1", "tp1_erase", 101},
        {"testplugins1", "tp1_rotate", 200},
        
        /* TestPlugins2 commands */
        {"testplugins2", "tp2_help", 0},
        {"testplugins2", "tp2_version", 2},
        {"testplugins2", "tp2_phong", 300},
        {"testplugins2", "tp2_raytrace", 400},
        
        /* TestPlugins3 commands */
        {"testplugins3", "tp3_help", 0},
        {"testplugins3", "tp3_version", 3},
        {"testplugins3", "tp3_overlap", 500},
        {"testplugins3", "tp3_volume", 600}
    };
    
    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        CommandTest& ct = tests[i];
        
        /* Find the library */
        LibraryAPI* lib = nullptr;
        for (auto& l : libraries) {
            if (strcmp(l.name, ct.lib_name) == 0) {
                lib = &l;
                break;
            }
        }
        TEST_ASSERT(lib != nullptr, "Library should exist");
        
        /* Check command exists */
        int exists = lib->cmd_exists(ct.cmd_name);
        TEST_ASSERT(exists == 1, "Command should exist");
        
        /* Execute command */
        int result = 0;
        int status = lib->cmd_run(ct.cmd_name, &result);
        TEST_ASSERT(status == 0, "Command execution should succeed");
        TEST_ASSERT_EQUAL(ct.expected_result, result, "Command should return expected value");
    }
    
    printf("  ✓ All commands executed successfully with correct return values\n");
    TEST_PASS();
}

/* Test: Verify library isolation (no cross-library interference) */
static bool test_library_isolation(std::vector<LibraryAPI>& libraries) {
    TEST_START("Library Isolation (No Cross-Library Interference)");
    
    /* Verify that TestPlugins1 commands don't exist in TestPlugins2 or TestPlugins3 */
    LibraryAPI* tp2_lib = nullptr;
    LibraryAPI* tp3_lib = nullptr;
    
    for (auto& l : libraries) {
        if (strcmp(l.name, "testplugins2") == 0) tp2_lib = &l;
        if (strcmp(l.name, "testplugins3") == 0) tp3_lib = &l;
    }
    
    TEST_ASSERT(tp2_lib != nullptr && tp3_lib != nullptr, "Libraries should exist");
    
    /* TestPlugins1 commands should not exist in TestPlugins2 */
    TEST_ASSERT(tp2_lib->cmd_exists("tp1_draw") == 0, "TestPlugins1 command should not exist in TestPlugins2");
    TEST_ASSERT(tp2_lib->cmd_exists("tp1_rotate") == 0, "TestPlugins1 command should not exist in TestPlugins2");
    
    /* TestPlugins2 commands should not exist in TestPlugins3 */
    TEST_ASSERT(tp3_lib->cmd_exists("tp2_phong") == 0, "TestPlugins2 command should not exist in TestPlugins3");
    TEST_ASSERT(tp3_lib->cmd_exists("tp2_raytrace") == 0, "TestPlugins2 command should not exist in TestPlugins3");
    
    /* TestPlugins3 commands should not exist in TestPlugins2 */
    TEST_ASSERT(tp2_lib->cmd_exists("tp3_overlap") == 0, "TestPlugins3 command should not exist in TestPlugins2");
    TEST_ASSERT(tp2_lib->cmd_exists("tp3_volume") == 0, "TestPlugins3 command should not exist in TestPlugins2");
    
    printf("  ✓ Each library maintains independent command registry\n");
    printf("  ✓ No cross-library command interference detected\n");
    TEST_PASS();
}

/* Test: Command name collision isolation */
static bool test_command_name_collisions(std::vector<LibraryAPI>& libraries) {
    TEST_START("Command Name Collision Isolation");
    
    printf("  Testing that libraries with colliding command names get their own implementations...\n\n");
    
    /* Test colliding command "test_common" - should exist in all libraries with different return values */
    printf("  Testing 'test_common' command (exists in all 3 libraries):\n");
    for (auto& lib : libraries) {
        int exists = lib.cmd_exists("test_common");
        TEST_ASSERT(exists == 1, "Command 'test_common' should exist in each library");
        
        bu_plugin_cmd_impl fn = lib.cmd_get("test_common");
        TEST_ASSERT(fn != nullptr, "Should be able to get 'test_common' command");
        
        int result = fn();
        
        /* Verify each library gets its own implementation */
        if (strcmp(lib.name, "testplugins1") == 0) {
            TEST_ASSERT(result == 1001, "testplugins1 'test_common' should return 1001");
            printf("    ✓ testplugins1: test_common returned %d (correct)\n", result);
        } else if (strcmp(lib.name, "testplugins2") == 0) {
            TEST_ASSERT(result == 1002, "testplugins2 'test_common' should return 1002");
            printf("    ✓ testplugins2: test_common returned %d (correct)\n", result);
        } else if (strcmp(lib.name, "testplugins3") == 0) {
            TEST_ASSERT(result == 1003, "testplugins3 'test_common' should return 1003");
            printf("    ✓ testplugins3: test_common returned %d (correct)\n", result);
        }
    }
    
    /* Test colliding command "draw" - should exist in all libraries with different return values */
    printf("\n  Testing 'draw' command (exists in all 3 libraries):\n");
    for (auto& lib : libraries) {
        int exists = lib.cmd_exists("draw");
        TEST_ASSERT(exists == 1, "Command 'draw' should exist in each library");
        
        bu_plugin_cmd_impl fn = lib.cmd_get("draw");
        TEST_ASSERT(fn != nullptr, "Should be able to get 'draw' command");
        
        int result = fn();
        
        /* Verify each library gets its own implementation */
        if (strcmp(lib.name, "testplugins1") == 0) {
            TEST_ASSERT(result == 2001, "testplugins1 'draw' should return 2001");
            printf("    ✓ testplugins1: draw returned %d (correct)\n", result);
        } else if (strcmp(lib.name, "testplugins2") == 0) {
            TEST_ASSERT(result == 2002, "testplugins2 'draw' should return 2002");
            printf("    ✓ testplugins2: draw returned %d (correct)\n", result);
        } else if (strcmp(lib.name, "testplugins3") == 0) {
            TEST_ASSERT(result == 2003, "testplugins3 'draw' should return 2003");
            printf("    ✓ testplugins3: draw returned %d (correct)\n", result);
        }
    }
    
    printf("\n  ✓ Command name collisions properly isolated across libraries\n");
    printf("  ✓ Each library gets its own correct implementation despite name collisions\n");
    TEST_PASS();
}

/* Test: Proper shutdown ordering */
static bool test_shutdown_ordering(std::vector<LibraryAPI>& libraries) {
    TEST_START("Proper Shutdown Ordering");
    
    printf("  Shutting down libraries in reverse load order...\n");
    
    /* Shutdown in reverse order (LIFO) */
    for (auto it = libraries.rbegin(); it != libraries.rend(); ++it) {
        printf("    Shutting down %s library...\n", it->name);
        it->shutdown();
        printf("      ✓ %s shutdown complete\n", it->name);
    }
    
    printf("  ✓ All libraries shut down successfully\n");
    TEST_PASS();
}

/* Test: Unload libraries */
static bool test_unload_libraries(std::vector<LibraryAPI>& libraries) {
    TEST_START("Unload Libraries");
    
    printf("  Unloading libraries in reverse order...\n");
    
    /* Unload in reverse order (LIFO) */
    for (auto it = libraries.rbegin(); it != libraries.rend(); ++it) {
        printf("    Unloading %s library...\n", it->name);
        UNLOAD_LIBRARY(it->handle);
        printf("      ✓ %s unloaded\n", it->name);
    }
    
    libraries.clear();
    
    printf("  ✓ All libraries unloaded successfully\n");
    TEST_PASS();
}

/* Main test runner */
int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("  Multi-Library Plugin Stress Test\n");
    printf("========================================\n");
    printf("\nThis test validates:\n");
    printf("  • Loading multiple independent libraries with separate plugin ecosystems\n");
    printf("  • Macro namespace isolation (BU_PLUGIN_NAME)\n");
    printf("  • Plugin loading and command execution for each library\n");
    printf("  • Library isolation (no cross-library interference)\n");
    printf("  • Proper shutdown and unload ordering\n");
    printf("\n");
    
    /* Get build configuration from command line (for multi-config generators) */
    if (argc > 1) {
        g_build_config = argv[1];
        printf("Build configuration: %s\n\n", g_build_config.c_str());
    }
    
    std::vector<LibraryAPI> libraries;
    
    /* Run all tests */
    test_load_libraries(libraries);
    test_initialize_libraries(libraries);
    test_load_plugins(libraries);
    test_execute_commands(libraries);
    test_library_isolation(libraries);
    test_command_name_collisions(libraries);
    test_shutdown_ordering(libraries);
    test_unload_libraries(libraries);
    
    /* Print summary */
    printf("\n========================================\n");
    printf("    Test Summary\n");
    printf("========================================\n");
    printf("Tests run:    %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("========================================\n");
    
    if (tests_failed == 0) {
        printf("\n✓ SUCCESS: All multi-library stress tests passed!\n");
        printf("  The plugin system correctly handles multiple independent\n");
        printf("  libraries with separate plugin ecosystems in the same\n");
        printf("  application, with proper initialization, execution, and\n");
        printf("  shutdown ordering.\n\n");
    }
    
    /* Return 0 if all tests passed, 1 otherwise */
    return (tests_failed == 0) ? 0 : 1;
}
