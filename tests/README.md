# Tests

## Unit Tests

### test_network_calculator.py
Tests IPv4 subnet calculation functions:
- Valid/invalid IPv4 addresses
- CIDR /0, /24, /31, /32
- Network/broadcast/usable address calculations
- Private address classification
- Special address types (loopback, multicast, etc.)

### test_electronics_calculator.py
Tests electronics calculation functions:
- Ohm's law (V=IR, I=V/R, R=V/I)
- Power, series/parallel resistance
- LED resistor estimation
- Voltage divider
- Capacitor unit conversion
- Frequency/period/duty cycle
- Division by zero handling

## Running Tests

```bash
cd tests
python3 test_network_calculator.py
python3 test_electronics_calculator.py
```

## Hardware Testing

Manual testing on physical Flipper Zero required for:
- Serial Commander (UART communication)
- GPIO Laboratory (pin I/O)
- Storage persistence
- Module navigation