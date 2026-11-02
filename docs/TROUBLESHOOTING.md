# Flipper Toolbox - Troubleshooting Guide

## Build Issues

### uFBT not found
```
Problem: command not found: ufbt
Solution: python -m pip install ufbt
```
### Toolchain download fails
```
Problem: Toolchain extraction error / download corrupted
Solution: rm -rf ~/.ufbt && ufbt --version (retries download)
```
### Compilation error: implicit declaration
```
Problem: Warning treated as error (-Werror)
Solution: Check API usage against SDK headers, add forward declarations
```
### Icon not found
```
Problem: flipper_toolbox.png not found
Solution: Place icon in project root directory (same as application.fam)
```

## Installation Issues

### App not appearing in menu
```
Problem: Flipper Toolbox not visible after install
Solution: 
1. Restart Flipper Zero
2. Check: Main Menu → Tools → Flipper Toolbox
3. Verify FAP copied to /ext/apps/Tools/ on SD card
```
### Incompatible firmware error
```
Problem: App won't launch
Solution: Update Flipper Zero firmware to 1.5.1+ via qFlipper
```
### FAP install failed
```
Problem: Installation rejected
Solution: Check SD card space, try different USB port/cable
```

## Runtime Issues

### App crashes on launch
```
Problem: Immediate exit to main menu
Solution: 
1. Check firmware version compatibility
2. Try clean build: ufbt clean && ufbt release
3. Delete app data: /int/flipper_toolbox/ on SD card
```

### Serial Commander not working
```
Problem: No data received / cannot connect
Solution:
1. Verify wiring: PB6→RX, PB7→RX, GND→GND
2. Check voltage: Target must be 3.3V TTL, not RS-232
3. Try different baud rate
4. Use loopback test: connect TX to RX on Flipper
```

### GPIO not responding
```
Problem: Read always shows LOW
Solution:
1. Check pin is configured as input
2. Verify external connection not shorted to GND
3. Test with external pull-up/down resistor
```

### Storage not saving
```
Problem: Profiles/settings lost after restart
Solution:
1. Verify SD card mounted
2. Check /int/flipper_toolbox/ directory exists
3. Ensure write permissions on SD card
```

## Hardware Safety Issues

### Damaged GPIO pin
```
Symptoms: Pin stuck HIGH/LOW, device overheating
Action:
1. IMMEDIATELY disconnect all external connections
2. Power cycle Flipper Zero
3. Do NOT use affected pin
4. Check for voltage mismatch (>3.3V)
```

### RS-232 connection without level shifter
```
Risk: Permanent damage to Flipper Zero GPIO
Action: Use MAX3232 or similar level converter between RS-232 and Flipper
```

## Getting Help

1. Check docs/TROUBLESHOOTING.md
2. Search GitHub Issues for similar problems
3. Create new issue with:
   - Flipper Zero firmware version
   - Build command and output
   - Steps to reproduce
   - Hardware connections (if applicable)