#!/bin/bash
# build.sh - Linux/macOS build script for Flipper Toolbox

set -e

echo "=== Flipper Toolbox Build Script ==="

# Check Python
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 not found"
    exit 1
fi

# Install uFBT
echo "Installing uFBT..."
python3 -m pip install ufbt

# Build debug
echo "Building debug FAP..."
ufbt

# Build release
echo "Building release FAP..."
ufbt release

echo "=== Build Complete ==="
echo "Debug ELF: dist/debug/flipper_toolbox_d.elf"
echo "Release FAP: dist/flipper_toolbox.fap"

# Run tests
echo "Running unit tests..."
cd tests
python3 test_network_calculator.py
python3 test_electronics_calculator.py
cd ..

echo "=== All Done ==="