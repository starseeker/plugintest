# plugintest
Test cross platform plugin mechanisms

## Overview

This repository provides a minimal, self-contained example to iteratively test the BU plugin core pattern:

- A host library (`ged_host`) configured with the BU plugin core (using `BU_PLUGIN_*` macros in `include/ged_plugin.h`) and a built-in implementation (defined in `host/libged_init.cpp`).
- A dynamic plugin (`plugin/example`) exporting a manifest-array via `ged_plugin_info` that lists commands to register at load.
- A small executable (`run_host`) that loads the plugin, registers commands via the manifest, and runs one.

## Structure

- `CMakeLists.txt`: top-level build configuration. Builds `ged_host` (shared), `run_host` (executable), and `ged-example-plugin` (shared) under `plugin/example`.
- `include/bu_plugin_core.h`: a minimal BU plugin core header sufficient for testing (subset of the full design), featuring:
  - Required `BU_PLUGIN_*` macros
  - Types (`bu_plugin_cmd_impl`, `bu_plugin_cmd`)
  - Registry APIs (`register`, `exists`, `get`, `count`, `init`)
  - Always-on `REGISTER_BU_PLUGIN_COMMAND` macro (C++ path)
  - Built-in registry implementation (C++ only) guarded by `BU_PLUGIN_IMPLEMENTATION`
  - Dynamic plugin manifest helpers (`bu_plugin_manifest`, `BU_PLUGIN_DECLARE_MANIFEST`, validation and registration helpers)
- `include/ged_plugin.h`: wrapper configuring `BU_PLUGIN_*` for the test "ged" host, with minimal function signature `int (*)(void)`
- `host/libged_init.cpp`: instantiates the built-in implementation and provides a minimal dynamic loader which resolves the manifest symbol (`ged_plugin_info`), validates, and registers commands.
- `host/exec.cpp`: test runner executable that optionally loads a plugin from the command line, reports registry size, and runs the "example" command.
- `plugin/example/example_plugin.cpp`: a trivial plugin implementing one command named "example" with signature `int (*)(void)`, exporting a manifest via `ged_plugin_info`.

## How to build & run

```bash
mkdir build && cd build
cmake .. && cmake --build .
```

On Linux/macOS:
```bash
./run_host ./plugin/example/libged-example-plugin.so
# or on macOS: ./run_host ./plugin/example/libged-example-plugin.dylib
```

On Windows:
```cmd
run_host.exe plugin\example\ged-example-plugin.dll
```

## Expected output

```
Initial registered count: 0
Registered 1 command(s) from ./plugin/example/libged-example-plugin.so
Final registered count: 1
Running 'example' command...
Hello from the example plugin!
Command 'example' returned: 42
```
