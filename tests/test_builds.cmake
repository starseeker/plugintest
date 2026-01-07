# test_builds.cmake - CMake script to test various build configurations
# This script can be executed as a CTest test to validate different CMake configurations
# Usage: cmake -P test_builds.cmake
# Or via CTest: add_test(NAME build_configs COMMAND ${CMAKE_COMMAND} -P test_builds.cmake)

# Get the project directory (parent of tests directory where this script is located)
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# Track results
set(PASSED 0)
set(FAILED 0)
set(CONFIGS_RESULTS "")

# Color output support (only works on terminals that support ANSI codes)
if(NOT WIN32)
    string(ASCII 27 ESC)
    set(COLOR_RED "${ESC}[0;31m")
    set(COLOR_GREEN "${ESC}[0;32m")
    set(COLOR_YELLOW "${ESC}[1;33m")
    set(COLOR_RESET "${ESC}[0m")
else()
    set(COLOR_RED "")
    set(COLOR_GREEN "")
    set(COLOR_YELLOW "")
    set(COLOR_RESET "")
endif()

# Function to run a build configuration
function(run_config CONFIG_NAME)
    # Parse additional CMake arguments
    set(CMAKE_ARGS ${ARGN})
    
    message("")
    message("========================================")
    message("${COLOR_YELLOW}Building configuration: ${CONFIG_NAME}${COLOR_RESET}")
    message("CMake args: ${CMAKE_ARGS}")
    message("========================================")
    
    # Create build directory name from config name (replace spaces with underscores)
    string(REPLACE " " "_" BUILD_DIR_SUFFIX "${CONFIG_NAME}")
    set(BUILD_DIR "${PROJECT_DIR}/build_${BUILD_DIR_SUFFIX}")
    
    # Remove old build directory and create new one
    file(REMOVE_RECURSE "${BUILD_DIR}")
    file(MAKE_DIRECTORY "${BUILD_DIR}")
    
    # Run CMake configure
    execute_process(
        COMMAND ${CMAKE_COMMAND} ${PROJECT_DIR} ${CMAKE_ARGS}
        WORKING_DIRECTORY ${BUILD_DIR}
        RESULT_VARIABLE CMAKE_RESULT
        OUTPUT_VARIABLE CMAKE_OUTPUT
        ERROR_VARIABLE CMAKE_ERROR
    )
    
    if(NOT CMAKE_RESULT EQUAL 0)
        message("${COLOR_RED}FAILED (cmake): ${CONFIG_NAME}${COLOR_RESET}")
        math(EXPR FAILED "${FAILED} + 1")
        set(FAILED ${FAILED} PARENT_SCOPE)
        set(CONFIGS_RESULTS "${CONFIGS_RESULTS}  ${CONFIG_NAME}: FAIL (cmake)\n" PARENT_SCOPE)
        return()
    endif()
    
    # Run build
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${BUILD_DIR}
        RESULT_VARIABLE BUILD_RESULT
        OUTPUT_VARIABLE BUILD_OUTPUT
        ERROR_VARIABLE BUILD_ERROR
    )
    
    if(NOT BUILD_RESULT EQUAL 0)
        message("${COLOR_RED}FAILED (build): ${CONFIG_NAME}${COLOR_RESET}")
        math(EXPR FAILED "${FAILED} + 1")
        set(FAILED ${FAILED} PARENT_SCOPE)
        set(CONFIGS_RESULTS "${CONFIGS_RESULTS}  ${CONFIG_NAME}: FAIL (build)\n" PARENT_SCOPE)
        return()
    endif()
    
    # Run tests
    message("${COLOR_YELLOW}Running tests for: ${CONFIG_NAME}${COLOR_RESET}")
    
    # Determine test harness path based on platform
    if(WIN32)
        set(TEST_HARNESS "${BUILD_DIR}/tests/test_harness.exe")
    else()
        set(TEST_HARNESS "${BUILD_DIR}/tests/test_harness")
    endif()
    
    execute_process(
        COMMAND ${TEST_HARNESS} ${BUILD_DIR}
        WORKING_DIRECTORY ${BUILD_DIR}
        RESULT_VARIABLE TEST_RESULT
        OUTPUT_VARIABLE TEST_OUTPUT
        ERROR_VARIABLE TEST_ERROR
    )
    
    if(NOT TEST_RESULT EQUAL 0)
        message("${COLOR_RED}FAILED (tests): ${CONFIG_NAME}${COLOR_RESET}")
        math(EXPR FAILED "${FAILED} + 1")
        set(FAILED ${FAILED} PARENT_SCOPE)
        set(CONFIGS_RESULTS "${CONFIGS_RESULTS}  ${CONFIG_NAME}: FAIL (tests)\n" PARENT_SCOPE)
        return()
    endif()
    
    # Success!
    message("${COLOR_GREEN}PASSED: ${CONFIG_NAME}${COLOR_RESET}")
    math(EXPR PASSED "${PASSED} + 1")
    set(PASSED ${PASSED} PARENT_SCOPE)
    set(CONFIGS_RESULTS "${CONFIGS_RESULTS}  ${CONFIG_NAME}: PASS\n" PARENT_SCOPE)
endfunction()

# Main test execution
message("========================================")
message("Plugin System Build Configuration Tests")
message("========================================")

# Test 1: Default Release build
run_config("Release" -DCMAKE_BUILD_TYPE=Release)

# Test 2: Debug build
run_config("Debug" -DCMAKE_BUILD_TYPE=Debug)

# Test 3: Release with strict warnings
run_config("Release-StrictWarnings" -DCMAKE_BUILD_TYPE=Release -DENABLE_STRICT_WARNINGS=ON)

# Test 4: Debug with strict warnings
run_config("Debug-StrictWarnings" -DCMAKE_BUILD_TYPE=Debug -DENABLE_STRICT_WARNINGS=ON)

# Test 5: RelWithDebInfo
run_config("RelWithDebInfo" -DCMAKE_BUILD_TYPE=RelWithDebInfo)

# Test 6: MinSizeRel
run_config("MinSizeRel" -DCMAKE_BUILD_TYPE=MinSizeRel)

# Test 7: No warnings (minimal build)
run_config("Release-NoWarnings" -DCMAKE_BUILD_TYPE=Release -DENABLE_STRICT_WARNINGS=OFF)

# Test 8: With sanitizers (Linux/macOS only)
if(UNIX)
    run_config("Debug-Sanitizers" -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON)
endif()

# Summary
message("")
message("========================================")
message("Build Configuration Test Summary")
message("========================================")
message("${COLOR_GREEN}Passed: ${PASSED}${COLOR_RESET}")
message("${COLOR_RED}Failed: ${FAILED}${COLOR_RESET}")
message("")
message("Individual results:")
message("${CONFIGS_RESULTS}")
message("")

# Exit with appropriate code
if(FAILED GREATER 0)
    message(FATAL_ERROR "Some build configuration tests failed")
endif()

message("All build configuration tests passed!")
