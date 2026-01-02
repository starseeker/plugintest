# Multi-Library Plugin System Stress Test

This directory contains a comprehensive stress test that validates the plugin system's ability to handle multiple independent libraries with their own plugin ecosystems running simultaneously in the same application.

## Overview

This test simulates a real-world scenario where an application uses multiple independent libraries (like `libged`, `librt`, and `libanalyze` in BRL-CAD), each with its own plugin ecosystem, all running in the same process.

## Architecture

### Independent Libraries

Three independent libraries are created, each with its own plugin system:

1. **libged** (Geometry Editing Library)
   - Namespace: `ged`
   - Built-in commands: `ged_help`, `ged_version`, `ged_status`
   - Plugins: `ged-draw-plugin`, `ged-edit-plugin`

2. **librt** (Ray Tracing Library)
   - Namespace: `rt`
   - Built-in commands: `rt_help`, `rt_version`, `rt_status`
   - Plugins: `rt-shader-plugin`, `rt-render-plugin`

3. **libanalyze** (Analysis Library)
   - Namespace: `analyze`
   - Built-in commands: `analyze_help`, `analyze_version`, `analyze_status`
   - Plugins: `analyze-overlap-plugin`, `analyze-volume-plugin`

### Key Features Tested

#### 1. Namespace Isolation via BU_PLUGIN_NAME Macro

Each library is compiled with a different `BU_PLUGIN_NAME` macro value:
```cpp
// libged
#define BU_PLUGIN_NAME ged

// librt
#define BU_PLUGIN_NAME rt

// libanalyze
#define BU_PLUGIN_NAME analyze
```

This ensures that plugin manifests are exported with library-specific symbol names:
- `ged_plugin_info()` for GED plugins
- `rt_plugin_info()` for RT plugins
- `analyze_plugin_info()` for ANALYZE plugins

#### 2. Independent Plugin Registries

Each library maintains its own independent command registry. Commands registered in one library do not appear in another library's registry, preventing cross-library interference.

#### 3. Proper Initialization Order

The stress test initializes all libraries and verifies that:
- Each library can be initialized independently
- Built-in commands are registered correctly
- Initial state is as expected (3 built-in commands per library)

#### 4. Plugin Loading

The test loads multiple plugins for each library and verifies:
- Correct number of commands registered
- No symbol collisions between plugins of different libraries
- Plugin manifest validation (ABI version, struct size)

#### 5. Command Execution

The test executes commands from all libraries and verifies:
- Commands execute correctly with expected return values
- No cross-library command execution
- Built-in and plugin commands both work

#### 6. Library Isolation

Critical test: Verifies that commands from one library do not leak into another:
- GED commands (`ged_draw`, `ged_rotate`) do not exist in RT or ANALYZE
- RT commands (`rt_phong`, `rt_raytrace`) do not exist in GED or ANALYZE
- ANALYZE commands (`analyze_overlap`, `analyze_volume`) do not exist in GED or RT

#### 7. Shutdown Ordering

The test verifies proper LIFO (Last In, First Out) shutdown ordering:
1. Shutdown libraries in reverse load order (ANALYZE → RT → GED)
2. Unload libraries in reverse order
3. Verify clean shutdown with no crashes or memory leaks

## Test Results

When run successfully, the test outputs:

```
========================================
  Multi-Library Plugin Stress Test
========================================

This test validates:
  • Loading multiple independent libraries with separate plugin ecosystems
  • Macro namespace isolation (BU_PLUGIN_NAME)
  • Plugin loading and command execution for each library
  • Library isolation (no cross-library interference)
  • Proper shutdown and unload ordering

...

Tests run:    7
Tests passed: 7
Tests failed: 0
========================================

✓ SUCCESS: All multi-library stress tests passed!
  The plugin system correctly handles multiple independent
  libraries with separate plugin ecosystems in the same
  application, with proper initialization, execution, and
  shutdown ordering.
```

## Directory Structure

```
multilib_stress/
├── CMakeLists.txt                          # Main build configuration
├── README.md                               # This file
├── libged/                                 # GED library
│   ├── CMakeLists.txt
│   └── ged_plugin_host.cpp
├── librt/                                  # RT library
│   ├── CMakeLists.txt
│   └── rt_plugin_host.cpp
├── libanalyze/                             # ANALYZE library
│   ├── CMakeLists.txt
│   └── analyze_plugin_host.cpp
├── plugins/
│   ├── ged/                                # GED plugins
│   │   ├── CMakeLists.txt
│   │   ├── ged_draw_plugin.cpp
│   │   └── ged_edit_plugin.cpp
│   ├── rt/                                 # RT plugins
│   │   ├── CMakeLists.txt
│   │   ├── rt_shader_plugin.cpp
│   │   └── rt_render_plugin.cpp
│   └── analyze/                            # ANALYZE plugins
│       ├── CMakeLists.txt
│       ├── analyze_overlap_plugin.cpp
│       └── analyze_volume_plugin.cpp
└── stress_test/
    ├── CMakeLists.txt
    └── multilib_stress_test.cpp            # Main test executable
```

## Building and Running

From the build directory:

```bash
# Build everything
cmake .. && cmake --build .

# Run the multi-library stress test
./multilib_stress/stress_test/multilib_stress_test
```

## Implementation Details

### Library Implementation Pattern

Each library follows this pattern:

1. **Define namespace**: Set `BU_PLUGIN_NAME` before including `bu_plugin.h`
2. **Implement host API**: Wrapper functions for the library's public API
3. **Register built-in commands**: Use `REGISTER_BU_PLUGIN_COMMAND` for built-ins
4. **Provide init/shutdown**: Initialize and cleanup the plugin system

Example from `ged_plugin_host.cpp`:
```cpp
#define BU_PLUGIN_NAME ged
#define BU_PLUGIN_IMPLEMENTATION
#include "bu_plugin.h"

extern "C" BU_PLUGIN_EXPORT int ged_init(void) {
    return bu_plugin_init();
}

static int ged_help(void) {
    printf("GED: Built-in help command\n");
    return 0;
}
REGISTER_BU_PLUGIN_COMMAND("ged_help", ged_help);
```

### Plugin Implementation Pattern

Each plugin follows this pattern:

1. **Define namespace**: Set `BU_PLUGIN_NAME` to match the target library
2. **Implement commands**: Define command functions
3. **Create manifest**: Define commands array and manifest structure
4. **Export manifest**: Use `BU_PLUGIN_DECLARE_MANIFEST` macro

Example from `ged_draw_plugin.cpp`:
```cpp
#define BU_PLUGIN_NAME ged
#include "bu_plugin.h"

static int ged_draw(void) {
    printf("GED Plugin: draw command executed\n");
    return 100;
}

static bu_plugin_cmd s_commands[] = {
    { "ged_draw", ged_draw }
};

static bu_plugin_manifest s_manifest = {
    "ged-draw-plugin",
    1,
    1,
    s_commands,
    BU_PLUGIN_ABI_VERSION,
    sizeof(bu_plugin_manifest)
};

BU_PLUGIN_DECLARE_MANIFEST(s_manifest)
```

## Why This Test Matters

This stress test validates the most complex real-world scenario for the plugin system:

1. **Real Application Pattern**: Applications like BRL-CAD use multiple independent libraries, each with plugins
2. **Symbol Isolation**: Ensures no symbol collisions between libraries
3. **Independent Registries**: Each library maintains its own command registry
4. **Clean Shutdown**: Verifies proper resource cleanup and ordering
5. **Macro Flexibility**: Validates the `BU_PLUGIN_NAME` macro system works correctly

Without this test, we could not confidently claim that the plugin system supports multiple independent libraries in the same application.
