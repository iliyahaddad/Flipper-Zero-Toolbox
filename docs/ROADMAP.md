# Flipper Toolbox - Roadmap

## Version 0.2.0 (Current)
- [x] Research report completed
- [x] Application manifest (application.fam)
- [x] Main menu with 9 items (8 modules + About)
- [x] Module navigation with back-button support
- [x] Network Calculator - IPv4 CIDR calculations
- [x] Electronics Calculator - 11 calculation types
- [x] Serial Commander - UART terminal with logging
- [x] GPIO Laboratory - Pin read/write/test with safety warnings
- [x] Device Profiles - Persistent storage CRUD
- [x] Session Logs - Log browser with delete confirmation
- [x] Protocol Learning - Educational reference (7 topics)
- [x] Settings - Persistent preferences
- [x] About screen with version info
- [x] Build system (uFBT)
- [x] FAP artifact produced

## Version 0.3.0 (Planned)
- [ ] Serial Commander: Full duplex verification
- [ ] Serial Commander: Session logging with metadata
- [ ] Device Profiles: Import/export functionality
- [ ] Session Logs: Filtering by module/type
- [ ] Settings: Diagnostics screen
- [ ] Improved error messages
- [ ] Code coverage improvement

## Version 0.4.0 (Planned)
- [ ] Network Calculator: MAC address validation
- [ ] Electronics Calculator: More component calculators
- [ ] Serial Commander: Predefined command presets
- [ ] GPIO Laboratory: Pin function reference
- [ ] Protocol Learning: Sample file inspection

## Version 0.5.0 (Planned)
- [ ] External hardware bridge documentation (ESP32 UART)
- [ ] Wi-Fi diagnostics via external bridge (documented only)
- [ ] Additional protocol analysis modules
- [ ] Performance optimization

## Long-term (1.0.0)
- [ ] Full hardware testing documentation
- [ ] Community-contributed modules
- [ ] Plugin architecture for extensibility
- [ ] Multi-language support

## Not Planned (API Limitations)
- ❌ Wi-Fi scanning (no Wi-Fi hardware on Flipper Zero)
- ❌ MQTT/HTTP networking (no IP stack in FAP API)
- ❌ Direct RS-232 voltage support (requires external level shifter)
- ❌ Custom firmware modifications
- ❌ Unauthorized security tools (credential theft, replay attacks)