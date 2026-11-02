# Changelog

## [0.2.0] - 2026-11-02

### Added
- Initial release of Flipper Toolbox
- 8 modules: Serial Commander, GPIO Laboratory, Network Calculator, Electronics Calculator, Device Profiles, Session Logs, Protocol Learning, Settings
- Main menu with navigation and About screen
- Binary file storage for settings and profiles
- Build system with uFBT

### Network Calculator
- IPv4 address validation
- CIDR notation support (0-32)
- Network address, broadcast address calculation
- Usable host count (/31 and /32 edge cases handled)
- Wildcard mask calculation
- Private vs public address classification
- Special address types (loopback, link-local, multicast, etc.)

### Electronics Calculator
- Ohm's law (V=IR, I=V/R, R=V/I)
- Power calculation (P=VI)
- Series and parallel resistance
- LED series resistor estimation
- Voltage divider calculation
- Capacitor unit conversion (pF, nF, µF, mF, F)
- Frequency/period conversion
- Duty cycle calculation

### Serial Commander
- UART terminal via USART1 (PB6 TX, PB7 RX)
- Baud rates: 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
- ASCII and HEX display modes
- Line endings: None, CR, LF, CRLF
- Bounded receive buffer (1024 bytes)
- Session logging (opt-in)

### GPIO Laboratory
- Pin selection from actual GPIO definitions
- Digital input read (HIGH/LOW)
- Digital output write (push-pull, open-drain)
- Output toggle and test patterns
- Safety warning on entry
- Pin restoration to analog on exit

### Device Profiles
- Create, view, delete profiles
- Persistent binary storage
- Profile types: Serial, GPIO, Network, Electronics
- Max 16 profiles

### Session Logs
- List saved log files
- View log contents
- Delete with confirmation
- Empty state handling

### Protocol Learning
- 7 topics: IR, NFC/RFID, Sub-GHz, UART, Digital Signals, HEX/Binary, Sample Files
- Educational content only

### Settings
- Default baud rate, display mode, line ending, confirm delete
- Persistent storage

## [0.1.0] - Planned (not released)
- Minimum viable application with main menu and About screen