# Third-Party Notices

Flipper Toolbox uses the following third-party components:

## 1. Flipper Zero Firmware APIs
- **Source**: https://github.com/flipperdevices/flipperzero-firmware
- **License**: MIT License
- **Usage**: GUI services, storage, Furi HAL (serial, GPIO), notification, record system
- **Modifications**: None - used as-is via header declarations

## 2. uFBT (Flipper Build Tool)
- **Source**: https://github.com/flipperdevices/flipperzero-ufbt
- **License**: MIT License
- **Usage**: Build system for FAP packaging
- **Modifications**: None - used as build tool only

## 3. Standard C Library
- **Source**: ARM GCC / newlib
- **License**: Various (GCC Runtime Library Exception)
- **Usage**: Standard C functions (snprintf, strncpy, malloc, etc.)
- **Modifications**: None

## License Summary

| Component | License | Copyright |
|-----------|---------|-----------|
| Flipper Zero Firmware APIs | MIT | Flipper Devices |
| uFBT Build Tool | MIT | Flipper Devices |
| Standard C Library | GCC RLE | Free Software Foundation |

The Flipper Toolbox application code is original and does not incorporate code from other open-source projects beyond the official firmware API headers.
