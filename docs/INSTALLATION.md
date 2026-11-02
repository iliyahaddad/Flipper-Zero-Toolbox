# Flipper Toolbox - Installation Guide

## Prerequisites

### Hardware
- Flipper Zero device (any hardware revision)
- USB-C cable for data transfer
- Firmware version **1.5.1 or later** (API 88+)

### Software (for building from source)
- **Python 3.10+**
- **uFBT** (`pip install ufbt`)
- **Git** (for cloning)

## Installation Methods

### Method 1: Pre-built FAP (Recommended)
1. Download latest `flipper_toolbox.fap` from [Releases](https://github.com/iliyahaddad/flipper-zero-toolbox/releases)
2. Connect Flipper Zero via USB
3. Run: `ufbt launch /path/to/flipper_toolbox.fap`
   - Or copy to SD card: `/ext/apps/Tools/flipper_toolbox.fap`
4. Launch from **Main Menu → Tools → Flipper Toolbox**

### Method 2: Build from Source
```bash
# Clone repository
git clone https://github.com/iliyahaddad/flipper-zero-toolbox.git
cd flipper-toolbox

# Install build tool
pip install ufbt

# Build and install
ufbt launch
```

### Method 3: qFlipper (GUI)
1. Open qFlipper
2. Go to **Apps** tab
3. Click **+** (Add app)
4. Select `flipper_toolbox.fap`
5. Click **Install**

## Verification

After installation:
1. Navigate to **Main Menu → Tools**
2. Find **Flipper Toolbox** (icon: toolbox)
3. Launch - should show main menu with 9 items

## Updating

### Via uFBT
```bash
cd flipper-toolbox
git pull
ufbt launch
```

### Via qFlipper
1. Remove old version in qFlipper Apps tab
2. Install new FAP

## Uninstalling

### Via uFBT
```bash
ufbt uninstall flipper_toolbox
```

### Via qFlipper
1. Apps tab → Flipper Toolbox → Remove

### Manual (SD Card)
Delete `/ext/apps/Tools/flipper_toolbox.fap` from SD card

## Firmware Compatibility

| Firmware | API Version | Status |
|----------|-------------|--------|
| 1.5.1+   | 88.2        | ✅ Fully supported |
| 1.4.x    | 87.x        | ⚠️ May work, untested |
| <1.4     | <87         | ❌ Not supported |

## Troubleshooting Installation

| Issue | Solution |
|-------|----------|
| "App not found in menu" | Restart Flipper Zero; check Tools category |
| "Incompatible firmware" | Update Flipper Zero firmware via qFlipper |
| "FAP install failed" | Check SD card space; try different USB cable |
| "App crashes on launch" | Report issue with firmware version |

## Building for Development

### Debug Build (with symbols)
```bash
ufbt
# Output: dist/debug/flipper_toolbox_d.elf
```

### Release Build (optimized)
```bash
ufbt release
# Output: dist/flipper_toolbox.fap
```

### Clean Build
```bash
ufbt clean
ufbt
```

## CI/CD Artifacts

GitHub Actions automatically builds on every push:
- **Debug ELF**: For debugging with GDB
- **Release FAP**: For distribution
- **Compile Commands**: For IDE support (clangd)

Download from Actions artifacts or Releases page.
