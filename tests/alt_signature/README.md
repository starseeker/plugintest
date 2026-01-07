# Alternative Signature Test

This test demonstrates complete integration of the `bu_plugin.h` system with custom command signatures beyond the default `int (*)(void)`.

## Purpose

The test validates that:
1. The plugin system fully supports custom function signatures via `BU_PLUGIN_CMD_RET` and `BU_PLUGIN_CMD_ARGS` macros
2. All core APIs work correctly with custom signatures (register, get, exists, count, foreach, load)
3. Dynamic plugins can be loaded with custom signatures
4. Built-in commands can be registered at static initialization with custom signatures
5. Applications can provide signature-specific wrapper functions for command execution

## Test Structure

### Host Library (`host/alt_sig_host.cpp`)

The host library:
- Defines custom signature `int (*)(int argc, const char** argv)` before including `bu_plugin.h`
- Implements `BU_PLUGIN_IMPLEMENTATION` with the custom signature
- Provides `alt_sig_cmd_run()` - a custom wrapper function for executing commands with arguments
- Registers built-in commands (`echo`, `count`) at static initialization time

### Plugins

Two plugins demonstrate the custom signature:

1. **args_plugin.cpp** - Provides:
   - `args_test`: Prints all arguments
   - `sum`: Sums numeric arguments
   - `concat`: Concatenates string arguments

2. **string_ops_plugin.cpp** - Provides:
   - `reverse`: Reverses each argument
   - `upper`: Converts arguments to uppercase
   - `length`: Counts total characters in all arguments

### Test Program (`test_alt_signature.cpp`)

The test program:
- Initializes the plugin system
- Tests built-in commands registered at static initialization
- Loads dynamic plugins with custom signatures
- Executes commands via the custom wrapper function
- Tests direct invocation via `bu_plugin_cmd_get()`
- Validates all API functions work correctly
- Performs comprehensive testing with 11 test cases

## Key Concepts Demonstrated

### Custom Signature Definition

```cpp
// Define BEFORE including bu_plugin.h
#define BU_PLUGIN_CMD_RET int
#define BU_PLUGIN_CMD_ARGS int argc, const char** argv
#include "bu_plugin.h"
```

### Custom Wrapper Function

Since `bu_plugin_cmd_run()` only works with the default signature `int (*)(void)`, applications using custom signatures must provide their own wrapper:

```cpp
extern "C" int alt_sig_cmd_run(const char *name, int argc, const char** argv, int *result) {
    bu_plugin_cmd_impl fn = bu_plugin_cmd_get(name);
    if (!fn) return -1;
    
    try {
        int ret = fn(argc, argv);  // Call with custom arguments
        if (result) *result = ret;
        return 0;
    } catch (...) {
        return -2;
    }
}
```

### Direct Command Invocation

Commands can also be invoked directly without a wrapper:

```cpp
bu_plugin_cmd_impl fn = bu_plugin_cmd_get("sum");
if (fn) {
    const char* args[] = {"100", "200"};
    int result = fn(2, args);  // Direct call
}
```

## Building and Running

The test is built as part of the main CMake build and added to CTest:

```bash
mkdir build && cd build
cmake ..
make test_alt_signature
ctest -R test_alt_signature --output-on-failure
```

Or run directly:

```bash
./tests/alt_signature/test_alt_signature
```

## Expected Output

The test performs 11 comprehensive tests covering:
- Built-in command execution
- Plugin loading
- Command execution from plugins
- Direct invocation
- Existence checks
- Command iteration

All tests should pass with output showing successful execution of each command.

## Integration with bu_plugin.h

This test demonstrates that `bu_plugin.h` is fully generalized to support custom signatures. The only API that is signature-specific is `bu_plugin_cmd_run()`, which is conditionally compiled only for the default signature. All other APIs work transparently with any signature:

- `bu_plugin_cmd_register()` - Works with any signature
- `bu_plugin_cmd_get()` - Works with any signature
- `bu_plugin_cmd_exists()` - Works with any signature
- `bu_plugin_cmd_count()` - Works with any signature
- `bu_plugin_cmd_foreach()` - Works with any signature
- `bu_plugin_load()` - Works with any signature
- `bu_plugin_init()` - Works with any signature

This design provides maximum flexibility while maintaining type safety.
