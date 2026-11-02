# Flipper Toolbox - Research Report

## Overview
This document records the research findings for the Flipper Toolbox project, a modular electronics, serial communication, and network toolkit for Flipper Zero.

## Primary Sources Investigated

### Official Flipper Zero Documentation
- **Applications API**: https://developer.flipper.net/flipperzero/doxygen/applications.html
- **First App Tutorial**: https://developer.flipper.net/flipperzero/doxygen/app_dev_your_first_app_in_c.html
- **Doxygen Reference**: https://developer.flipper.net/flipperzero/doxygen/

### Development Tools
- **uFBT (Flipper Build Tool)**: https://github.com/flipperdevices/flipperzero-ufbt
- **Official Firmware**: https://github.com/flipperdevices/flipperzero-firmware
- **Application Examples**: https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications

### Additional Resources
- **Flipper Devices GitHub**: https://github.com/flipperdevices

## Development Environment Verification

### Installed Tools
- **Python 3.11**: Installed via winget
- **uFBT**: Installed via pip (`pip install ufbt`)
- **ARM GCC Toolchain**: Auto-downloaded by uFBT on first build
- **SDK Version**: Flipper Zero Firmware 1.5.1 (API version 88.2)

### Build Verification
- **Target**: f7 (Flipper Zero)
- **Build Type**: FAP (Flipper Application Package)
- **Output**: `dist/flipper_toolbox.fap` (45 KB)

## API Compatibility Analysis

### Verified APIs (Working in FAP)
| API | Header | Status |
|-----|--------|--------|
| GUI (ViewDispatcher, Widget, VariableItemList, Submenu, TextInput) | `gui/modules/*.h` | ✅ Working |
| Storage (File, Dir, common) | `storage/storage.h` | ✅ Working |
| Notification (sequence_success, sequence_error, sequence_single_vibro) | `notification/notification_messages.h` | ✅ Working |
| Furi HAL Serial (USART control, async RX/TX) | `furi_hal_serial.h`, `furi_hal_serial_control.h` | ✅ Working |
| Furi HAL GPIO (pin init, read, write, modes) | `furi_hal_gpio.h`, `furi_hal_resources.h` | ✅ Working |
| Furi Record (open/close) | `furi/record.h` | ✅ Working |

### API Limitations Identified
1. **Serial Framing Configuration**: `furi_hal_serial_configure_framing()` exists but data bits/parity/stop bits not exposed in UI
2. **GPIO Pin Enumeration**: Limited to pins exposed via `furi_hal_resources_pin_by_number()` (32 pins)
3. **Text Input**: Only `TextInputCallback` (void*) supported, not button-style callbacks
4. **Widget Text**: Uses `widget_add_text_box_element()` for multi-line, `widget_add_string_element()` for single-line
3. **No strcat/sprintf**: Standard C string functions disabled in firmware API (use snprintf)
4. **No dynamic allocation in hot paths**: All buffers statically allocated

### Features Requiring Firmware Modification (Not Implemented)
- Wi-Fi networking (requires ESP32 bridge via UART)
- Full RS-232 voltage level support (requires external level shifter hardware)
- Sub-GHz/NFC/IR protocol parsing (existing apps handle this)

## Hardware Limitations

### Flipper Zero GPIO
- **Logic Level**: 3.3V only (NOT 5V tolerant)
- **Available Pins**: GPIO on external connector (PC0, PC1, PC3, PB2, PB3, PA4, PA6, PA7, etc.)
- **UART Pins**: USART1 TX=PB6, RX=PB7 (via `gpio_usart_tx`/`gpio_usart_rx`)
- **Current Limit**: ~20mA per pin, ~80mA total

### Safety Requirements Documented
- ⚠️ Never connect directly to RS-232 (±12V) - requires MAX3232 or similar level converter
- ⚠️ Never connect to mains voltage
- ⚠️ Verify pinout before connecting external devices
- ⚠️ Restore pins to analog input on module exit

## Reusable Open-Source Components

### From Official Firmware Examples
- **Application structure**: Based on `applications/user/` examples
- **GUI patterns**: VariableItemList, Widget, Submenu usage from built-in apps
- **Storage patterns**: Binary file I/O from settings/examples

### Algorithms Implemented (Original)
- IPv4 subnet calculation (bit manipulation, no floating point)
- Electronics calculations (Ohm's law, power, parallel/series, LED resistor, voltage divider)
- Capacitor unit conversion (pF, nF, µF, mF, F)
- Frequency/period/duty cycle conversion

### No External Libraries Copied
All code written specifically for this project using official Flipper Zero APIs.

### Third-Party Notices
- Flipper Zero Firmware APIs: Used under MIT License (flipperdevices/flipperzero-firmware)
- uFBT Build Tool: Used under MIT License (flipperdevices/flipperzero-ufbt)

## Features Implemented as Standalone FAP

| Module | Status | Notes |
|--------|--------|-------|
| Serial Commander | ✅ Complete | UART terminal with logging |
| GPIO Laboratory | ✅ Complete | Pin read/write/test with safety |
| Network Calculator | ✅ Complete | IPv4 CIDR calculations |
| Electronics Calculator | ✅ Complete | 11 calculation types |
| Device Profiles | ✅ Complete | Binary storage, CRUD |
| Session Logs | ✅ Complete | File browser with delete |
| Protocol Learning | ✅ Complete | Educational reference |
| Settings | ✅ Complete | Persistent preferences |

## Features Requiring External Hardware (Future)
- ESP32 Wi-Fi bridge (UART communication protocol to be defined)
- External USB-UART adapters
- GPIO expansion boards

## Features Not Implemented (API Unavailable)
- Wi-Fi scanning (no Wi-Fi hardware)
- MQTT/HTTP networking (no IP stack)
- Full protocol decoding (use existing Flipper apps)

## Conclusion
All 8 required modules implemented using verified official APIs. The application compiles successfully, produces a valid FAP, and follows Flipper Zero development best practices. No firmware modifications required.