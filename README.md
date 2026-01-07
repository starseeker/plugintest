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

All code is organized under the `tests/` directory for a clean, consolidated testing infrastructure:

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

### Host Components (tests/host/)

- `tests/host/libbu_init.cpp`: instantiates the built-in implementation, registers built-in commands (help, version, status), and provides a minimal dynamic loader.
- `tests/host/exec.cpp`: test runner executable that optionally loads a plugin from the command line, reports registry size, and runs the "example" command.

### Plugins (tests/plugin/)

- `tests/plugin/example/`: A trivial plugin implementing one command named "example"
- `tests/plugin/math_plugin/`: Plugin with multiple math commands (add, multiply, square)
- `tests/plugin/string_plugin/`: Plugin with string-related commands (length, upper)
- `tests/plugin/duplicate_plugin/`: Plugin that deliberately has a duplicate command name to test conflict handling
- `tests/plugin/stress_plugin/`: Plugin with 50 commands for stress testing
- `tests/plugin/large_plugin/`: Plugin with 500 commands for scalability testing
- `tests/plugin/c_only/`: A pure C plugin (no C++) to verify cross-platform C plugin support
- `tests/plugin/edge_cases/`: Edge case plugins for testing:
  - `empty_plugin`: Plugin with no commands
  - `null_impl_plugin`: Plugin with null implementations in some commands
  - `special_names_plugin`: Plugin with special/long/unusual command names

### Test Framework

All testing infrastructure is consolidated under the `tests/` directory with a clean, minimal test set:

**Core Test Executables (4 tests):**

1. **`tests/test_harness.cpp`** - Comprehensive plugin system testing
   - **Plugin Loading**: Single plugins, multiple plugins, all plugins simultaneously
   - **Command Testing**: Registration, execution, lookup, enumeration (foreach)
   - **Edge Cases**: Empty manifests, null implementations, special/long command names
   - **Duplicate Handling**: Duplicate commands across plugins, duplicate registration attempts
   - **API Validation**: Null parameters, invalid paths, error handling
   - **Built-in Commands**: Help, version, status commands
   - **Stress Testing**: 50 commands (stress plugin), 500 commands (large plugin)
   - **C/C++ Interop**: Pure C plugins without C++
   - **Collision Protection**: All plugins loaded simultaneously without symbol conflicts

2. **`tests/test_robustness.cpp`** - Robustness and ABI validation
   - **Thread Safety**: Concurrent command registration and foreach enumeration
   - **ABI Validation**: Correct/incorrect version, struct size mismatches
   - **Error Handling**: Path policy, missing manifest symbols, error logging
   - **Exception Safety**: Exception handling in command execution
   - **Manifest Validation**: Duplicate detection, null implementation filtering

3. **`tests/alt_signature/`** - Alternative function signatures
   - Tests support for different function signatures beyond basic `int (*)(void)`

4. **`tests/multilib_stress/`** - Multi-library plugin ecosystems
   - **Three independent libraries**: Each with its own plugin system and namespace
   - **Namespace isolation**: No cross-library command interference
   - **Proper shutdown ordering**: LIFO (Last In, First Out) unload ordering
   - **Real-world scenario**: Multiple libraries with plugins in the same application

**Build Configuration Tests:**
- `tests/test_builds.sh`: Tests various build configurations (Release, Debug, RelWithDebInfo, MinSizeRel, AddressSanitizer)

**Test Plugins:**
- `tests/plugin/`: Example plugins for functional testing (example, math, string, c_only, stress, large, duplicate, edge_cases)
- `tests/plugins/`: Specialized plugins for ABI validation (test_bad_abi, test_bad_struct_size, test_no_manifest)

**Complete Coverage:**
This minimal test set eliminates duplication while maintaining complete coverage of all plugin system functionality, providing a clean, well-organized testing setup suitable for integration into BRL-CAD's testing infrastructure.

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
./run_bu_plugin ./tests/plugin/example/libbu-example-plugin.so
```

On macOS:
```bash
./run_bu_plugin ./tests/plugin/example/libbu-example-plugin.dylib
```

On Windows:
```cmd
run_bu_plugin.exe tests\plugin\example\bu-example-plugin.dll
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

The test suite provides comprehensive coverage with a clean, minimal set of tests:

```bash
ctest --output-on-failure
```

**Test Coverage (4 tests):**
- **`plugin_tests`**: Comprehensive test harness covering all plugins in `tests/plugin/` directory
  - Tests all plugins: example, math, string, c_only, stress (50 cmds), large (500 cmds), duplicate, edge_cases (empty, null_impl, special_names)
  - Validates plugin loading, command registration, execution, and edge cases
  - Tests API functionality: exists, get, register, count, foreach
  - Tests error handling: invalid paths, null parameters, duplicate registration
  - Tests built-in commands and command enumeration
  
- **`robustness_tests`**: Thread-safety, robustness, and ABI validation testing
  - Thread-safe concurrent command registration and enumeration
  - ABI version validation (correct/incorrect versions, struct size)
  - Error logging and path policy validation
  - Exception handling in command execution
  - Missing manifest symbol detection
  
- **`test_alt_signature`**: Alternative function signature testing
  - Validates support for different function signatures beyond the basic `int (*)(void)`
  
- **`multilib_stress_test`**: Multi-library plugin ecosystem testing
  - Tests multiple independent libraries with separate plugin systems
  - Validates namespace isolation and proper shutdown ordering

This minimal test set eliminates duplication while maintaining complete coverage of all plugin system functionality.

### Run all build configuration tests

```bash
./tests/test_builds.sh
```

## Expected output from run_bu_plugin

```
Initial registered count: 3
Registered 1 command(s) from ./tests/plugin/example/libbu-example-plugin.so
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
