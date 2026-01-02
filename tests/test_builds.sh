#!/bin/bash
# test_builds.sh - Test various build configurations
# This script builds and tests the plugin system with different compile flags

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Track results
PASSED=0
FAILED=0
CONFIGS=()

run_config() {
    local name="$1"
    shift
    local cmake_args="$@"
    
    echo ""
    echo "========================================"
    echo -e "${YELLOW}Building configuration: $name${NC}"
    echo "CMake args: $cmake_args"
    echo "========================================"
    
    local build_dir="$PROJECT_DIR/build_${name// /_}"
    rm -rf "$build_dir"
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    if cmake "$PROJECT_DIR" $cmake_args; then
        if cmake --build .; then
            echo -e "${YELLOW}Running tests for: $name${NC}"
            if ./tests/test_harness .; then
                echo -e "${GREEN}PASSED: $name${NC}"
                PASSED=$((PASSED + 1))
                CONFIGS+=("$name: PASS")
            else
                echo -e "${RED}FAILED (tests): $name${NC}"
                FAILED=$((FAILED + 1))
                CONFIGS+=("$name: FAIL (tests)")
            fi
        else
            echo -e "${RED}FAILED (build): $name${NC}"
            FAILED=$((FAILED + 1))
            CONFIGS+=("$name: FAIL (build)")
        fi
    else
        echo -e "${RED}FAILED (cmake): $name${NC}"
        FAILED=$((FAILED + 1))
        CONFIGS+=("$name: FAIL (cmake)")
    fi
    
    cd "$PROJECT_DIR"
}

echo "========================================"
echo "Plugin System Build Configuration Tests"
echo "========================================"

# Test 1: Default Release build
run_config "Release" -DCMAKE_BUILD_TYPE=Release

# Test 2: Debug build
run_config "Debug" -DCMAKE_BUILD_TYPE=Debug

# Test 3: Release with strict warnings
run_config "Release-StrictWarnings" -DCMAKE_BUILD_TYPE=Release -DENABLE_STRICT_WARNINGS=ON

# Test 4: Debug with strict warnings
run_config "Debug-StrictWarnings" -DCMAKE_BUILD_TYPE=Debug -DENABLE_STRICT_WARNINGS=ON

# Test 5: RelWithDebInfo
run_config "RelWithDebInfo" -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Test 6: MinSizeRel
run_config "MinSizeRel" -DCMAKE_BUILD_TYPE=MinSizeRel

# Test 7: No warnings (minimal build)
run_config "Release-NoWarnings" -DCMAKE_BUILD_TYPE=Release -DENABLE_STRICT_WARNINGS=OFF

# Test 8: With AddressSanitizer (if supported)
if [[ "$OSTYPE" == "linux-gnu"* ]] || [[ "$OSTYPE" == "darwin"* ]]; then
    run_config "Debug-Sanitizers" -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
fi

# Summary
echo ""
echo "========================================"
echo "Build Configuration Test Summary"
echo "========================================"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""
echo "Individual results:"
for config in "${CONFIGS[@]}"; do
    echo "  $config"
done
echo ""

if [ $FAILED -gt 0 ]; then
    exit 1
fi

exit 0
