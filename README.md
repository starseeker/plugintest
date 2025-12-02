# plugintest
Test cross platform plugin mechanisms

## Overview

This repository provides a comprehensive testing framework for the BU plugin core pattern:

- A host library (`ged_host`) configured with the BU plugin core (using `BU_PLUGIN_*` macros in `include/ged_plugin.h`) and built-in commands (defined in `host/libged_init.cpp`).
- Multiple dynamic plugins exporting manifest-arrays via `ged_plugin_info` that list commands to register at load.
- A small executable (`run_host`) that loads a plugin, registers commands via the manifest, and runs one.
- A comprehensive test harness (`test_harness`) that stress-tests the plugin system.

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
- `include/ged_plugin.h`: wrapper configuring `BU_PLUGIN_*` for the test "ged" host, with minimal function signature `int (*)(void)`
- `host/libged_init.cpp`: instantiates the built-in implementation, registers built-in commands (help, version, status), and provides a minimal dynamic loader.
- `host/exec.cpp`: test runner executable that optionally loads a plugin from the command line, reports registry size, and runs the "example" command.

### Plugins

- `plugin/example/`: A trivial plugin implementing one command named "example"
- `plugin/math_plugin/`: Plugin with multiple math commands (add, multiply, square)
- `plugin/string_plugin/`: Plugin with string-related commands (length, upper)
- `plugin/duplicate_plugin/`: Plugin that deliberately has a duplicate command name to test conflict handling
- `plugin/stress_plugin/`: Plugin with 50 commands for stress testing
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
- `tests/test_builds.sh`: Script to test various build configurations:
  - Release and Debug builds
  - Strict warning configurations
  - RelWithDebInfo and MinSizeRel
  - AddressSanitizer builds

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
./run_host ./plugin/example/libged-example-plugin.so
```

On macOS:
```bash
./run_host ./plugin/example/libged-example-plugin.dylib
```

On Windows:
```cmd
run_host.exe plugin\example\ged-example-plugin.dll
```

### Run the comprehensive test harness

```bash
./tests/test_harness .
```

### Run all build configuration tests

```bash
./tests/test_builds.sh
```

## Expected output from run_host

```
Initial registered count: 3
Registered 1 command(s) from ./plugin/example/libged-example-plugin.so
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
