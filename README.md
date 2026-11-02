# Flipper Toolbox

Modular Electronics, Serial Communication, and Network Toolkit for Flipper Zero

[![Build](https://github.com/iliyahaddad/flipper-zero-toolbox/actions/workflows/build.yml/badge.svg)](https://github.com/iliyahaddad/flipper-zero-toolbox/actions/workflows/build.yml)
[![Firmware: 1.5.1+](https://img.shields.io/badge/firmware-1.5.1+-green.svg)](https://github.com/flipperdevices/flipperzero-firmware)

## Overview

Flipper Toolbox is a standalone Flipper Application Package (FAP) that combines practical electronics and network troubleshooting tools into one maintainable application for Flipper Zero.

Built using official Flipper Zero firmware APIs and uFBT build system - no firmware modifications required.

## Features

| Module | Description |
|--------|-------------|
| **Serial Commander** | UART terminal with baud selection (1200-115200), ASCII/HEX display, line endings, session logging |
| **GPIO Laboratory** | Pin read/write, output toggle, test patterns, safety warnings |
| **Network Calculator** | IPv4 CIDR subnet calculations, network/broadcast/usable addresses |
| **Electronics Calculator** | Ohm's law, power, series/parallel resistors, LED resistor, voltage divider, capacitor conversion |
| **Device Profiles** | Persistent profile storage for different device setups |
| **Session Logs** | Log browser with view/delete functionality |
| **Protocol Learning** | Educational reference for IR, NFC, Sub-GHz, UART, digital signals |
| **Settings** | Persistent preferences (baud, display, line ending, confirm) |
| **About** | Version, license, repository information |

## Hardware Safety

⚠️ **Flipper Zero GPIO is 3.3V logic only.** Never connect directly to RS-232 (±12V) or 5V devices without appropriate level conversion. See `docs/HARDWARE.md` for wiring diagrams.

## Installation

### Pre-built FAP
Download the latest `flipper_toolbox.fap` from [Releases](https://github.com/your-repo/flipper-toolbox/releases) and install via:
```bash
ufbt launch /path/to/flipper_toolbox.fap
```

### Build from Source
```bash
# Install Python and uFBT
python -m pip install ufbt

# Clone and build
git clone https://github.com/your-repo/flipper-toolbox.git
cd flipper-toolbox
ufbt release
# Output: dist/flipper_toolbox.fap
```

### Launch on Device
```bash
ufbt launch
```

## Documentation

| Document | Location |
|----------|----------|
| Installation | `docs/INSTALLATION.md` |
| Hardware Reference | `docs/HARDWARE.md` |
| Architecture | `docs/ARCHITECTURE.md` |
| Development | `docs/DEVELOPMENT.md` |
| Testing | `docs/TESTING.md` |
| Troubleshooting | `docs/TROUBLESHOOTING.md` |
| Security | `docs/SECURITY.md` |
| Research | `docs/RESEARCH.md` |
| Roadmap | `docs/ROADMAP.md` |

## Testing

Automated unit tests for calculator logic:
```bash
cd tests
python3 test_network_calculator.py
python3 test_electronics_calculator.py
```

## License

See `LICENSE` file.

## Contributing

See `CONTRIBUTING.md` for guidelines.

## Acknowledgments

- Flipper Devices team for the Flipper Zero platform and open firmware
- uFBT build tool maintainers
- Open source community for Flipper Zero applications
