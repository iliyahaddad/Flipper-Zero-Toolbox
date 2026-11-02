# Flipper Toolbox - Hardware Reference

## Flipper Zero GPIO Pinout

### External Connector (Top Side)
```
Pin  Function      GPIO    Notes
────────────────────────────────────────
1    VCC           -       3.3V out (150mA max)
2    GND           -       Ground
3    PC0           PC0     GPIO / ADC
4    PC1           PC1     GPIO / ADC
5    PC3           PC3     GPIO / ADC
6    PB2           PB2     GPIO
7    PB3           PB3     GPIO
8    PA4           PA4     GPIO / ADC / DAC
9    PA6           PA6     GPIO / ADC
10   PA7           PA7     GPIO / ADC
11   5V            -       5V out (USB powered only)
12   GND           -       Ground
```

### Internal UART (USART1)
```
Function    Pin    GPIO    Notes
────────────────────────────────────────
TX          PB6    USART1_TX   Serial Commander output
RX          PB7    USART1_RX   Serial Commander input
```

### Safety Electrical Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Logic High | 3.3V | **NOT 5V tolerant** |
| Logic Low | 0V | |
| Max Output Current | 20mA/pin | 80mA total all GPIOs |
| Max Input Voltage | 3.3V | Absolute max 3.6V |
| ADC Resolution | 12-bit | 0-3.3V range |
| PWM Frequency | Up to 1MHz | Via TIM |

## Serial Commander Hardware Requirements

### For TTL UART (3.3V) Devices
```
Flipper Zero          Target Device
────────────────────────────────────────
PB6 (TX)  ──────────→ RX
PB7 (RX)  ←────────── TX
GND       ──────────→ GND
```

**Compatible with**: ESP32, STM32, Arduino (3.3V), Raspberry Pi GPIO, most modern MCUs

### For RS-232 Devices (±12V) - REQUIRES LEVEL CONVERTER
```
Flipper Zero          MAX3232          RS-232 Device
────────────────────────────────────────────────────
PB6 (TX)  ──────────→ T1IN  T1OUT ────→ RX
PB7 (RX)  ←────────── R1OUT R1IN  ←──── TX
GND       ──────────→ GND   GND  ─────→ GND
VCC (3.3V)──────────→ VCC   -      ───→ -
```

**Required Components**:
- MAX3232 or SP3232 level shifter (3.3V logic)
- 4x 0.1µF capacitors (for charge pump)
- DB9 connector or appropriate cable

### For 5V TTL Devices - REQUIRES LEVEL CONVERTER
```
Flipper Zero          BSS138/74LVC245   5V Device
────────────────────────────────────────────────────
PB6 (TX)  ──────────→ A → B ──────────→ RX
PB7 (RX)  ←────────── B ← A ────────── TX
GND       ──────────→ GND ────────────→ GND
```

## GPIO Laboratory Pin Usage

### Supported Pins (via furi_hal_resources_pin_by_number)
All 32 pins enumerated, but only external connector pins recommended for user access.

### Input Modes
- **Digital Input**: `GpioModeInput` (pull-up/down configurable)
- **Analog Input**: `GpioModeAnalog` (ADC via furi_hal_adc)
- **Interrupt**: `GpioModeInterruptRise/Fall/RiseFall`

### Output Modes
- **Push-Pull**: `GpioModeOutputPushPull` (drives high/low)
- **Open-Drain**: `GpioModeOutputOpenDrain` (needs pull-up)

## Voltage Level Compatibility Matrix

| Target Device | Flipper Direct | Level Converter Required |
|---------------|----------------|-------------------------|
| 3.3V MCU (ESP32, STM32) | ✅ Yes | No |
| 5V MCU (Arduino Uno) | ❌ No | Yes (TXS0108, BSS138) |
| RS-232 (PC serial) | ❌ No | Yes (MAX3232) |
| Raspberry Pi GPIO | ✅ Yes | No |
| Logic Analyzer (3.3V) | ✅ Yes | No |
| Logic Analyzer (5V) | ❌ No | Yes |

## Test Equipment Connections

### Oscilloscope/Logic Analyzer
```
Flipper Zero          Scope/LA
────────────────────────────────────
GPIO Pin (e.g. PC0) ──→ Channel 1
GND                   ──→ Ground
```

### Multimeter (Voltage Measurement)
```
Flipper Zero          Multimeter
────────────────────────────────────
GPIO Pin (Output)     ──→ V+ (DC Voltage)
GND                   ──→ COM
```

### Current Measurement (Series)
```
Flipper Zero          Multimeter          Load
────────────────────────────────────────────────
GPIO Pin (Output)     ──→ A (mA)    ──────→ Load + 
                                                    │
GND                   ◀─── COM        ◀───── Load -
```

## Cable Recommendations

### For Serial Commander
- **Jumper Wires**: Female-to-female Dupont (2.54mm)
- **Length**: <30cm for signal integrity at 115200 baud
- **Shielded**: Not required for short runs

### For GPIO Laboratory
- **Breadboard**: Standard 2.54mm pitch
- **Resistors**: 1kΩ-10kΩ for LED/current limiting
- **LEDs**: Standard 3mm/5mm with forward voltage <3.3V

## Power Considerations

### Flipper Zero Power Output
| Pin | Voltage | Max Current | Notes |
|-----|---------|-------------|-------|
| VCC (Pin 1) | 3.3V | 150mA | From internal regulator |
| 5V (Pin 11) | 5V | 500mA | Only when USB connected |
| GPIOs | 3.3V | 20mA each | Not for powering external devices |

### Do NOT Power From Flipper
- Relays, motors, high-power LEDs
- Other development boards
- Sensors requiring >20mA

## Emergency Recovery

### If GPIO Damaged
1. Disconnect all external connections
2. Restart Flipper Zero (power cycle)
3. Run GPIO Laboratory → Test Pattern on affected pin
4. If pin stuck HIGH/LOW: may need firmware recovery

### Firmware Recovery
1. Hold **Back + Up** while powering on
2. Connect to qFlipper
3. Flash official firmware

## Quick Reference Card

```
SERIAL COMMANDER (USART1):
  TX = PB6 (Pin on external header via level shifter)
  RX = PB7 (Pin on external header via level shifter)
  GND = Any GND pin
  ⚠️ 3.3V LOGIC ONLY - USE LEVEL SHIFTER FOR RS-232/5V!

GPIO LAB (External Connector):
  PC0, PC1, PC3, PB2, PB3, PA4, PA6, PA7
  ⚠️ 3.3V LOGIC - MAX 20mA PER PIN
  ⚠️ SET MODE TO ANALOG BEFORE CONNECTING NEW DEVICE
```