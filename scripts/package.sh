#!/bin/bash
# package.sh - Package Flipper Toolbox for distribution

set -e

VERSION="0.2.0"
PACKAGE_DIR="dist"

echo "=== Flipper Toolbox Packaging ==="

# Ensure release build exists
if [ ! -f "dist/flipper_toolbox.fap" ]; then
    echo "Release FAP not found. Building..."
    ufbt release
fi

# Create package directory
PKG_NAME="flipper-toolbox-v${VERSION}"
rm -rf "${PKG_NAME}"
mkdir -p "${PKG_NAME}"

# Copy files
cp dist/flipper_toolbox.fap "${PKG_NAME}/"
cp LICENSE "${PKG_NAME}/"
cp README.md "${PKG_NAME}/"
cp CHANGELOG.md "${PKG_NAME}/"

# Create tar.gz
tar czf "${PKG_NAME}.tar.gz" "${PKG_NAME}/"
rm -rf "${PKG_NAME}"

echo "=== Package Created: ${PKG_NAME}.tar.gz ==="