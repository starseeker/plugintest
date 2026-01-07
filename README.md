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

All testing infrastructure is organized under the `tests/` directory and provides comprehensive coverage of both the `plugin/` and `host/` directory components:

**Core Test Executables:**
- `tests/test_harness.cpp`: Comprehensive test harness that tests all plugins from `plugin/` directory:
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
  - Scalability testing with 500 commands
  - C-only plugins (pure C without C++)
  - Collision protection with all plugins loaded simultaneously
  
- `tests/test_robustness.cpp`: Thread-safety and robustness testing

**Host Executable Tests:**
- CTest integration tests for `run_bu_plugin` (from `host/exec.cpp`) with all plugins:
  - Example plugin test
  - Math plugin test
  - String plugin test
  - C-only plugin test
  - Stress plugin test (50 commands)
  - Large plugin test (500 commands)
  - Duplicate plugin test
  - Empty plugin test
  - Null implementation test
  - Special names plugin test
  - No plugin test (baseline)

**Build and Configuration Tests:**
- `tests/test_builds.sh`: Script to test various build configurations:
  - Release and Debug builds
  - Strict warning configurations
  - RelWithDebInfo and MinSizeRel
  - AddressSanitizer builds

**Specialized Tests:**
- `tests/alt_signature/`: Alternative function signature testing
- `tests/plugins/`: Test-specific plugins for ABI validation:
  - `test_bad_abi`: Plugin with wrong ABI version
  - `test_bad_struct_size`: Plugin with wrong struct size
  - `test_no_manifest`: Plugin missing manifest symbol
- `tests/multilib_stress/`: A comprehensive stress test that validates multiple independent libraries with separate plugin ecosystems:
  - **Three independent libraries**: `libtestplugins1`, `libtestplugins2`, `libtestplugins3` - each with its own plugin system
  - **Namespace isolation**: Each library uses `BU_PLUGIN_NAME` macro to namespace its plugins (`testplugins1`, `testplugins2`, `testplugins3`)
  - **Independent plugin ecosystems**: Each library has 2 plugins with multiple commands
  - **Library isolation testing**: Verifies no cross-library command interference
  - **Proper shutdown ordering**: Tests LIFO (Last In, First Out) unload ordering
  - **Macro define testing**: Validates `BU_PLUGIN_NAME` macro correctly namespaces symbols
  
  This test represents the full stress scenario encountered in real applications where multiple independent libraries with their own plugins interact in the same executable.

**Complete Coverage:**
The test suite ensures that all components from both `plugin/` and `host/` directories are thoroughly tested, providing a clean, well-organized testing setup suitable for integration into BRL-CAD's testing infrastructure.

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
./tests/multilib_stress/stress_test/multilib_stress_test
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

### Run all tests via CTest

The test suite provides comprehensive coverage of all plugin and host components:

```bash
ctest --output-on-failure
```

**Test Coverage:**
- **`plugin_tests`**: Comprehensive test harness covering all plugins in `plugin/` directory
- **`robustness_tests`**: Thread-safety and robustness testing
- **`test_alt_signature`**: Alternative function signature testing
- **`multilib_stress_test`**: Multi-library plugin ecosystem testing
- **`run_bu_plugin_*`**: Tests for the host executable with all plugins:
  - `run_bu_plugin_example`: Tests example plugin
  - `run_bu_plugin_math`: Tests math plugin
  - `run_bu_plugin_string`: Tests string plugin
  - `run_bu_plugin_c_only`: Tests C-only plugin
  - `run_bu_plugin_stress`: Tests stress plugin (50 commands)
  - `run_bu_plugin_large`: Tests large plugin (500 commands)
  - `run_bu_plugin_duplicate`: Tests duplicate command handling
  - `run_bu_plugin_empty`: Tests empty plugin
  - `run_bu_plugin_null_impl`: Tests null implementation handling
  - `run_bu_plugin_special_names`: Tests special command names
  - `run_bu_plugin_no_plugin`: Tests executable without plugin

Total: 15 comprehensive tests covering all aspects of the plugin system.

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
