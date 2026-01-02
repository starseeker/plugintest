# Multi-library stress test build configuration
#
# This builds:
#   - Three independent libraries (libged, librt, libanalyze) each with own plugin system
#   - Plugins for each library (2 plugins per library)
#   - A comprehensive stress test executable
#
# The stress test validates:
#   - Multiple libraries can coexist with separate plugin ecosystems
#   - BU_PLUGIN_NAME macro properly namespaces plugins
#   - No cross-library interference
#   - Proper initialization and shutdown ordering

# Build the three independent libraries
add_subdirectory(libged)
add_subdirectory(librt)
add_subdirectory(libanalyze)

# Build plugins for each library
add_subdirectory(plugins/ged)
add_subdirectory(plugins/rt)
add_subdirectory(plugins/analyze)

# Build the stress test executable
add_subdirectory(stress_test)
