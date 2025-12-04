/**
 * bu_plugin_core.h - Minimal BU plugin core header for testing.
 *
 * This header provides:
 *   - Required BU_PLUGIN_* macros for cross-platform symbol export/import
 *   - Core types: bu_plugin_cmd_impl (function pointer), bu_plugin_cmd (command descriptor)
 *   - Registry APIs: register, exists, get, count, init
 *   - REGISTER_BU_PLUGIN_COMMAND macro for C++ built-in command registration
 *   - Built-in registry implementation (C++ only) guarded by BU_PLUGIN_IMPLEMENTATION
 *   - Dynamic plugin manifest helpers: bu_plugin_manifest, BU_PLUGIN_DECLARE_MANIFEST
 */

#ifndef BU_PLUGIN_CORE_H
#define BU_PLUGIN_CORE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Cross-platform symbol visibility macros.
 * Define BU_PLUGIN_EXPORT for exporting symbols from shared libraries,
 * and BU_PLUGIN_IMPORT for importing them.
 */
#if defined(_WIN32) || defined(__CYGWIN__)
#  ifdef BU_PLUGIN_BUILDING_DLL
#    define BU_PLUGIN_EXPORT __declspec(dllexport)
#  else
#    define BU_PLUGIN_EXPORT __declspec(dllimport)
#  endif
#  define BU_PLUGIN_IMPORT __declspec(dllimport)
#  define BU_PLUGIN_LOCAL
#else
#  if __GNUC__ >= 4
#    define BU_PLUGIN_EXPORT __attribute__ ((visibility ("default")))
#    define BU_PLUGIN_IMPORT __attribute__ ((visibility ("default")))
#    define BU_PLUGIN_LOCAL  __attribute__ ((visibility ("hidden")))
#  else
#    define BU_PLUGIN_EXPORT
#    define BU_PLUGIN_IMPORT
#    define BU_PLUGIN_LOCAL
#  endif
#endif

/*
 * BU_PLUGIN_API: Use on declarations/definitions that are part of the public plugin API.
 * When building the host library (BU_PLUGIN_IMPLEMENTATION defined), this exports symbols.
 * When building a plugin or client code, this imports them.
 */
#ifdef BU_PLUGIN_IMPLEMENTATION
#  define BU_PLUGIN_API BU_PLUGIN_EXPORT
#else
#  define BU_PLUGIN_API BU_PLUGIN_IMPORT
#endif

/*
 * Type definitions for plugin commands.
 */

/**
 * bu_plugin_cmd_impl - Function pointer type for a plugin command implementation.
 * This is a default implementation; the host can define its own typedef
 * before including this header by defining BU_PLUGIN_CMD_IMPL_DEFINED.
 * For this test, it's simply: int (*)(void)
 */
#ifndef BU_PLUGIN_CMD_IMPL_DEFINED
typedef int (*bu_plugin_cmd_impl)(void);
#define BU_PLUGIN_CMD_IMPL_DEFINED
#endif

/**
 * bu_plugin_cmd - Descriptor for a single plugin command.
 */
typedef struct bu_plugin_cmd {
    const char *name;           /* Command name */
    bu_plugin_cmd_impl impl;    /* Function pointer to implementation */
} bu_plugin_cmd;

/**
 * bu_plugin_manifest - Descriptor for a plugin's exported commands.
 */
typedef struct bu_plugin_manifest {
    const char *plugin_name;    /* Name of the plugin */
    unsigned int version;       /* Plugin manifest version (for compatibility checks) */
    unsigned int cmd_count;     /* Number of commands in the commands array */
    const bu_plugin_cmd *commands;  /* Array of command descriptors */
} bu_plugin_manifest;

/*
 * Registry APIs - Declared here, implemented in the host library.
 */

/**
 * bu_plugin_cmd_register - Register a command with the global registry.
 * @param name  The command name.
 * @param impl  The function pointer for the command implementation.
 * @return 0 on success, non-zero on failure (e.g., duplicate name).
 */
BU_PLUGIN_API int bu_plugin_cmd_register(const char *name, bu_plugin_cmd_impl impl);

/**
 * bu_plugin_cmd_exists - Check if a command is registered.
 * @param name  The command name to check.
 * @return 1 if the command exists, 0 otherwise.
 */
BU_PLUGIN_API int bu_plugin_cmd_exists(const char *name);

/**
 * bu_plugin_cmd_get - Retrieve a command's implementation.
 * @param name  The command name to look up.
 * @return The function pointer, or NULL if not found.
 */
BU_PLUGIN_API bu_plugin_cmd_impl bu_plugin_cmd_get(const char *name);

/**
 * bu_plugin_cmd_count - Get the number of registered commands.
 * @return The count of registered commands.
 */
BU_PLUGIN_API size_t bu_plugin_cmd_count(void);

/**
 * bu_plugin_cmd_foreach - Iterate over all registered commands in sorted order.
 * @param callback  Function called for each command with (name, impl, user_data).
 * @param user_data Opaque pointer passed to callback.
 *
 * Useful for listing all available commands (e.g., for help systems).
 * Commands are iterated in alphabetical order by name for stable output.
 * The callback should return 0 to continue, non-zero to stop iteration.
 */
typedef int (*bu_plugin_cmd_callback)(const char *name, bu_plugin_cmd_impl impl, void *user_data);
BU_PLUGIN_API void bu_plugin_cmd_foreach(bu_plugin_cmd_callback callback, void *user_data);

/**
 * bu_plugin_init - Initialize the plugin registry (call once at startup).
 * @return 0 on success.
 *
 * Note: All registry operations are thread-safe, protected by a mutex.
 */
BU_PLUGIN_API int bu_plugin_init(void);

/**
 * bu_plugin_load - Load a dynamic plugin from a shared library path.
 * @param path  Path to the shared library (.so, .dylib, .dll).
 * @return Number of commands registered from the plugin, or -1 on error.
 *
 * Note: Plugins are kept loaded for the lifetime of the process. There is
 * currently no bu_plugin_unload() function. This is intentional for simplicity
 * and safety - unloading code that may have function pointers still in use
 * is error-prone.
 */
BU_PLUGIN_API int bu_plugin_load(const char *path);

#ifdef __cplusplus
} /* extern "C" */
#endif

/*
 * C++ helper macro for registering built-in commands at static initialization time.
 * Usage: REGISTER_BU_PLUGIN_COMMAND("cmdname", my_cmd_func);
 * This creates a static object whose constructor registers the command.
 * Uses __COUNTER__ for unique variable names to avoid collisions.
 */
#ifdef __cplusplus

/* Helper macros for unique identifier generation */
#define BU_PLUGIN_CONCAT_IMPL(a, b) a##b
#define BU_PLUGIN_CONCAT(a, b) BU_PLUGIN_CONCAT_IMPL(a, b)

#ifdef __COUNTER__
#define BU_PLUGIN_UNIQUE_ID BU_PLUGIN_CONCAT(_bu_plugin_cmd_registrar_, __COUNTER__)
#else
#define BU_PLUGIN_UNIQUE_ID BU_PLUGIN_CONCAT(_bu_plugin_cmd_registrar_, __LINE__)
#endif

namespace bu_plugin_detail {
    struct CommandRegistrar {
        CommandRegistrar(const char *name, bu_plugin_cmd_impl impl) {
            bu_plugin_cmd_register(name, impl);
        }
    };
}

#define REGISTER_BU_PLUGIN_COMMAND(name, impl) \
    static ::bu_plugin_detail::CommandRegistrar \
    BU_PLUGIN_UNIQUE_ID(name, impl)
#endif

/*
 * Macro for declaring the plugin manifest symbol in a dynamic plugin.
 * This exports a function that returns a pointer to the plugin's manifest.
 * Usage in plugin source:
 *   static bu_plugin_cmd s_commands[] = { {"example", example_impl}, ... };
 *   static bu_plugin_manifest s_manifest = { "myplugin", 1, 1, s_commands };
 *   BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
 *
 * The host will dlsym() for "ged_plugin_info" (or the host-specific symbol name).
 */
#ifdef __cplusplus
#define BU_PLUGIN_DECLARE_MANIFEST(manifest_var) \
    extern "C" BU_PLUGIN_EXPORT const bu_plugin_manifest* ged_plugin_info(void) { \
        return &(manifest_var); \
    }
#else
#define BU_PLUGIN_DECLARE_MANIFEST(manifest_var) \
    BU_PLUGIN_EXPORT const bu_plugin_manifest* ged_plugin_info(void) { \
        return &(manifest_var); \
    }
#endif

/*
 * Built-in registry implementation (C++ only).
 * This is included in the host library when BU_PLUGIN_IMPLEMENTATION is defined.
 */
#if defined(BU_PLUGIN_IMPLEMENTATION) && defined(__cplusplus)

#include <unordered_map>
#include <string>
#include <cstdio>
#include <mutex>
#include <vector>
#include <algorithm>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace bu_plugin_impl {

/**
 * Thread-safe registry using unordered_map for O(1) lookups.
 * Uses a static local to ensure initialization before first use.
 * Protected by a mutex for thread-safe access.
 */
static std::unordered_map<std::string, bu_plugin_cmd_impl>& get_registry() {
    static std::unordered_map<std::string, bu_plugin_cmd_impl> registry;
    return registry;
}

static std::mutex& get_mutex() {
    static std::mutex mtx;
    return mtx;
}

} /* namespace bu_plugin_impl */

extern "C" {

int bu_plugin_cmd_register(const char *name, bu_plugin_cmd_impl impl) {
    if (!name || !impl) return -1;
    if (name[0] == '\0') return -1;  /* Reject empty string names */
    std::lock_guard<std::mutex> lock(bu_plugin_impl::get_mutex());
    auto& reg = bu_plugin_impl::get_registry();
    if (reg.find(name) != reg.end()) {
        return -1; /* Duplicate */
    }
    reg[name] = impl;
    return 0;
}

int bu_plugin_cmd_exists(const char *name) {
    if (!name) return 0;
    std::lock_guard<std::mutex> lock(bu_plugin_impl::get_mutex());
    auto& reg = bu_plugin_impl::get_registry();
    return reg.find(name) != reg.end() ? 1 : 0;
}

bu_plugin_cmd_impl bu_plugin_cmd_get(const char *name) {
    if (!name) return nullptr;
    std::lock_guard<std::mutex> lock(bu_plugin_impl::get_mutex());
    auto& reg = bu_plugin_impl::get_registry();
    auto it = reg.find(name);
    return (it != reg.end()) ? it->second : nullptr;
}

size_t bu_plugin_cmd_count(void) {
    std::lock_guard<std::mutex> lock(bu_plugin_impl::get_mutex());
    return bu_plugin_impl::get_registry().size();
}

void bu_plugin_cmd_foreach(bu_plugin_cmd_callback callback, void *user_data) {
    if (!callback) return;
    std::lock_guard<std::mutex> lock(bu_plugin_impl::get_mutex());
    auto& reg = bu_plugin_impl::get_registry();
    
    /* Collect and sort command names for stable, predictable output order */
    std::vector<std::string> names;
    names.reserve(reg.size());
    for (const auto& pair : reg) {
        names.push_back(pair.first);
    }
    std::sort(names.begin(), names.end());
    
    /* Iterate in sorted order */
    for (const auto& name : names) {
        auto it = reg.find(name);
        if (it != reg.end()) {
            if (callback(it->first.c_str(), it->second, user_data) != 0) {
                break;  /* Callback requested stop */
            }
        }
    }
}

int bu_plugin_init(void) {
    /* No-op for now; registry is initialized on first access */
    return 0;
}

/**
 * bu_plugin_load - Load a dynamic plugin and register its commands.
 */
int bu_plugin_load(const char *path) {
    if (!path) return -1;

#if defined(_WIN32)
    HMODULE handle = LoadLibraryA(path);
    if (!handle) {
        fprintf(stderr, "Failed to load plugin: %s (error %lu)\n", path, GetLastError());
        return -1;
    }
    typedef const bu_plugin_manifest* (*info_fn)(void);
    info_fn get_info = (info_fn)GetProcAddress(handle, "ged_plugin_info");
    if (!get_info) {
        fprintf(stderr, "Plugin %s does not export ged_plugin_info\n", path);
        FreeLibrary(handle);
        return -1;
    }
#else
    void *handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        fprintf(stderr, "Failed to load plugin: %s (%s)\n", path, dlerror());
        return -1;
    }
    typedef const bu_plugin_manifest* (*info_fn)(void);
    info_fn get_info = (info_fn)dlsym(handle, "ged_plugin_info");
    if (!get_info) {
        fprintf(stderr, "Plugin %s does not export ged_plugin_info\n", path);
        dlclose(handle);
        return -1;
    }
#endif

    const bu_plugin_manifest *manifest = get_info();
    if (!manifest) {
        fprintf(stderr, "Plugin %s returned NULL manifest\n", path);
#if defined(_WIN32)
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif
        return -1;
    }

    /* Validate manifest */
    if (!manifest->commands || manifest->cmd_count == 0) {
        fprintf(stderr, "Plugin %s has no commands\n", path);
        /* Not an error, just nothing to register */
        return 0;
    }

    int registered = 0;
    for (unsigned int i = 0; i < manifest->cmd_count; i++) {
        const bu_plugin_cmd *cmd = &manifest->commands[i];
        if (cmd->name && cmd->impl) {
            if (bu_plugin_cmd_register(cmd->name, cmd->impl) == 0) {
                registered++;
            }
        }
    }

    return registered;
}

} /* extern "C" */

#endif /* BU_PLUGIN_IMPLEMENTATION && __cplusplus */

#endif /* BU_PLUGIN_CORE_H */
