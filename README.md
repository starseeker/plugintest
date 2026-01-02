# plugintest
Test cross platform plugin mechanisms

## Overview

This repository provides a comprehensive testing framework for the BU plugin core pattern:

- A host library (`bu_plugin_host`) configured with the BU plugin core (using `BU_PLUGIN_*` macros in `include/bu_plugin.h`) and built-in commands (defined in `host/libbu_init.cpp`).
- Multiple dynamic plugins exporting manifest-arrays via `bu_plugin_info` that list commands to register at load.
- A small executable (`run_bu_plugin`) that loads a plugin, registers commands via the manifest, and runs one.
- A comprehensive test harness (`test_harness`) that stress-tests the plugin system.
- **NEW**: A multi-library stress test (`multilib_stress_test`) that validates multiple independent libraries with their own plugin ecosystems running simultaneously in the same application.

## Structure

### Core Components

- `CMakeLists.txt`: top-level build configuration with support for various build types and compiler warning levels.
- `include/bu_plugin_core.h`: a minimal BU plugin core header sufficient for testing (subset of the full design), featuring:
  - Required `BU_PLUGIN_*` macros
  - Types (`bu_plugin_cmd_impl`, `bu_plugin_cmd`)
  - Registry APIs (`register`, `exists`, `get`, `count`, `init`)
  - Always-on `REGISTER_BU_PLUGIN_COMMAND` macro (C++ path)
  - Built-in registry implementation (C++ only) guarded by `BU_PLUGIN_IMPLEMENTATION`
  - Dynamic plugin manifest helpers (`bu_plugin_manifest`, `BU_PLUGIN_DECLARE_MANIFEST`, validation and registration helpers)
- `include/bu_plugin.h`: wrapper including `bu_plugin_core.h` for the test host, with minimal function signature `int (*)(void)`
- `host/libbu_init.cpp`: instantiates the built-in implementation, registers built-in commands (help, version, status), and provides a minimal dynamic loader.
- `host/exec.cpp`: test runner executable that optionally loads a plugin from the command line, reports registry size, and runs the "example" command.

### Plugins

- `plugin/example/`: A trivial plugin implementing one command named "example"
- `plugin/math_plugin/`: Plugin with multiple math commands (add, multiply, square)
- `plugin/string_plugin/`: Plugin with string-related commands (length, upper)
- `plugin/duplicate_plugin/`: Plugin that deliberately has a duplicate command name to test conflict handling
- `plugin/stress_plugin/`: Plugin with 50 commands for stress testing
- `plugin/c_only/`: A pure C plugin (no C++) to verify cross-platform C plugin support
- `plugin/edge_cases/`: Edge case plugins for testing:
  - `empty_plugin`: Plugin with no commands
  - `null_impl_plugin`: Plugin with null implementations in some commands
  - `special_names_plugin`: Plugin with special/long/unusual command names

### Test Framework

- `tests/test_harness.cpp`: Comprehensive test harness that tests:
  - Initial state verification
  - Built-in command functionality
  - Null API parameter handling
  - Duplicate registration handling
  - Invalid plugin path handling
  - Loading single and multiple plugins
  - Duplicate command name handling across plugins
  - Empty manifests
  - Null implementations
  - Special command names (very long names, underscores, numbers, mixed case)
  - Stress testing with 50 commands
  - C-only plugins (pure C without C++)
- `tests/test_builds.sh`: Script to test various build configurations:
  - Release and Debug builds
  - Strict warning configurations
  - RelWithDebInfo and MinSizeRel
  - AddressSanitizer builds

### Multi-Library Stress Test

- `multilib_stress/`: A comprehensive stress test that validates multiple independent libraries with separate plugin ecosystems:
  - **Three independent libraries**: `libged`, `librt`, `libanalyze` - each with its own plugin system
  - **Namespace isolation**: Each library uses `BU_PLUGIN_NAME` macro to namespace its plugins (`ged`, `rt`, `analyze`)
  - **Independent plugin ecosystems**: Each library has 2 plugins with multiple commands
  - **Library isolation testing**: Verifies no cross-library command interference
  - **Proper shutdown ordering**: Tests LIFO (Last In, First Out) unload ordering
  - **Macro define testing**: Validates `BU_PLUGIN_NAME` macro correctly namespaces symbols
  
  This test represents the full stress scenario encountered in real applications where multiple independent libraries with their own plugins interact in the same executable.

## How to build & run

### Basic Build

```bash
mkdir build && cd build
cmake .. && cmake --build .
```

### Build with Strict Warnings

```bash
cmake -DENABLE_STRICT_WARNINGS=ON -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### Build with Sanitizers (Linux/macOS)

```bash
cmake -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### Run the simple test

On Linux:
```bash
./run_bu_plugin ./plugin/example/libbu-example-plugin.so
```

On macOS:
```bash
./run_bu_plugin ./plugin/example/libbu-example-plugin.dylib
```

On Windows:
```cmd
run_bu_plugin.exe plugin\example\bu-example-plugin.dll
```

### Run the comprehensive test harness

```bash
./tests/test_harness .
```

### Run the multi-library stress test

This test validates multiple independent libraries with separate plugin ecosystems running in the same application:

```bash
./multilib_stress/stress_test/multilib_stress_test
```

Expected output:
```
========================================
  Multi-Library Plugin Stress Test
========================================
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

### Run all build configuration tests

```bash
./tests/test_builds.sh
```

## Expected output from run_bu_plugin

```
Initial registered count: 3
Registered 1 command(s) from ./plugin/example/libbu-example-plugin.so
Final registered count: 4
Running 'example' command...
Hello from the example plugin!
Command 'example' returned: 42
```

## Expected output from test_harness

```
========================================
    Plugin System Test Harness
========================================
Plugin directory: .

=== TEST: Initial State ===
  Initial command count: 3
  PASS

=== TEST: Built-in Commands ===
  ...
  PASS

... (more tests) ...

========================================
    Test Summary
========================================
Tests run:    13
Tests passed: 13
Tests failed: 0
========================================
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_STRICT_WARNINGS` | ON | Enable strict compiler warnings |
| `ENABLE_WERROR` | OFF | Treat warnings as errors |
| `ENABLE_SANITIZERS` | OFF | Enable AddressSanitizer and UndefinedBehaviorSanitizer |
| `CMAKE_BUILD_TYPE` | Release | Build type (Release, Debug, RelWithDebInfo, MinSizeRel) |
