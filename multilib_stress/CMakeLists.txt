# Multi-library stress test build configuration
#
# This builds:
#   - Three independent libraries (libtestplugins1, libtestplugins2, libtestplugins3) each with own plugin system
#   - Plugins for each library (2 plugins per library)
#   - A comprehensive stress test executable
#
# The stress test validates:
#   - Multiple libraries can coexist with separate plugin ecosystems
#   - BU_PLUGIN_NAME macro properly namespaces plugins
#   - No cross-library interference
#   - Proper initialization and shutdown ordering

# Build the three independent libraries
add_subdirectory(libtestplugins1)
add_subdirectory(libtestplugins2)
add_subdirectory(libtestplugins3)

# Build plugins for each library
add_subdirectory(plugins/testplugins1)
add_subdirectory(plugins/testplugins2)
add_subdirectory(plugins/testplugins3)

# Build the stress test executable
add_subdirectory(stress_test)
