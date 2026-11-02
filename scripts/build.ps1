<# 
# build.ps1 - PowerShell build script for Flipper Toolbox (Windows)
#

$ErrorActionPreference = "Stop"

Write-Host "=== Flipper Toolbox Build Script ===" -ForegroundColor Cyan

# Check Python
if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Error "Error: Python not found. Install from https://python.org or via winget"
    exit 1
}

# Install uFBT
Write-Host "Installing uFBT..." -ForegroundColor Yellow
python -m pip install ufbt

# Build debug
Write-Host "Building debug FAP..." -ForegroundColor Yellow
ufbt

# Build release
Write-Host "Building release FAP..." -ForegroundColor Yellow
ufbt release

Write-Host "=== Build Complete ===" -ForegroundColor Green
Write-Host "Debug ELF: dist/debug/flipper_toolbox_d.elf" -ForegroundColor Green
Write-Host "Release FAP: dist/flipper_toolbox.fap" -ForegroundColor Green

# Run tests
Write-Host "Running unit tests..." -ForegroundColor Yellow
Set-Location tests
python test_network_calculator.py
python test_electronics_calculator.py
Set-Location ..

Write-Host "=== All Done ===" -ForegroundColor Green