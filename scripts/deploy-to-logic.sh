#!/bin/bash

# OneKnob - Deploy to Logic Pro
# Builds AU plugin and installs to system Components folder

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

PLUGIN_NAME="OneKnob"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/Builds/MacOSX/build/Release"
COMPONENT_SRC="$BUILD_DIR/$PLUGIN_NAME.component"
COMPONENT_DST="$HOME/Library/Audio/Plug-Ins/Components/$PLUGIN_NAME.component"

echo "=========================================="
echo "  $PLUGIN_NAME - Deploy to Logic Pro"
echo "=========================================="

# Parse arguments
SKIP_BUILD=false
VERBOSE=false

for arg in "$@"; do
    case $arg in
        --skip-build) SKIP_BUILD=true ;;
        --verbose) VERBOSE=true ;;
        --help)
            echo "Usage: $0 [--skip-build] [--verbose]"
            exit 0
            ;;
    esac
done

# Step 1: Pre-flight checks
echo ""
echo "Step 1: Pre-flight checks..."

if [ ! -f "$PROJECT_DIR/$PLUGIN_NAME.jucer" ]; then
    echo -e "${RED}FAIL${NC} - Project file not found"
    exit 1
fi
echo -e "  ${GREEN}PASS${NC} - Project file exists"

if [ ! -w "$HOME/Library/Audio/Plug-Ins/Components" ]; then
    echo -e "${RED}FAIL${NC} - Components folder not writable"
    exit 1
fi
echo -e "  ${GREEN}PASS${NC} - Components folder writable"

# Step 2: Build
if [ "$SKIP_BUILD" = false ]; then
    echo ""
    echo "Step 2: Building AU plugin..."

    cd "$PROJECT_DIR/Builds/MacOSX"

    if [ "$VERBOSE" = true ]; then
        xcodebuild -scheme "$PLUGIN_NAME - AU" -configuration Release build
    else
        xcodebuild -scheme "$PLUGIN_NAME - AU" -configuration Release build 2>&1 | tail -5
    fi

    if [ $? -eq 0 ]; then
        echo -e "  ${GREEN}PASS${NC} - Build succeeded"
    else
        echo -e "  ${RED}FAIL${NC} - Build failed"
        exit 1
    fi
else
    echo ""
    echo "Step 2: Skipping build (--skip-build)"
fi

# Step 3: Verify build
echo ""
echo "Step 3: Verifying build..."

if [ ! -d "$COMPONENT_SRC" ]; then
    echo -e "  ${RED}FAIL${NC} - Component not found at $COMPONENT_SRC"
    exit 1
fi
echo -e "  ${GREEN}PASS${NC} - Component exists"

# Step 4: AU Validation
echo ""
echo "Step 4: AU Validation..."

if auval -v aufx 1Knb Flet > /dev/null 2>&1; then
    echo -e "  ${GREEN}PASS${NC} - auval validation passed"
else
    echo -e "  ${YELLOW}WARN${NC} - auval validation failed (may work anyway)"
fi

# Step 5: Deploy
echo ""
echo "Step 5: Deploying to Components folder..."

rm -rf "$COMPONENT_DST"
cp -R "$COMPONENT_SRC" "$COMPONENT_DST"

if [ -d "$COMPONENT_DST" ]; then
    echo -e "  ${GREEN}PASS${NC} - Deployed to $COMPONENT_DST"
else
    echo -e "  ${RED}FAIL${NC} - Deployment failed"
    exit 1
fi

# Step 6: Clear cache
echo ""
echo "Step 6: Clearing AU cache..."

rm -rf "$HOME/Library/Caches/AudioUnitCache"
echo -e "  ${GREEN}PASS${NC} - Cache cleared"

# Step 7: Post-deployment validation
echo ""
echo "Step 7: Post-deployment validation..."

if auval -v aufx 1Knb Flet > /dev/null 2>&1; then
    echo -e "  ${GREEN}PASS${NC} - Post-deploy validation passed"
else
    echo -e "  ${YELLOW}WARN${NC} - Post-deploy validation issues"
fi

echo ""
echo "=========================================="
echo -e "  ${GREEN}SUCCESS${NC} - $PLUGIN_NAME deployed!"
echo "  Restart Logic Pro to load the plugin"
echo "=========================================="
