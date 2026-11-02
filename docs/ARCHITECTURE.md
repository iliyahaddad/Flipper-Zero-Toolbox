# Flipper Toolbox - Architecture Documentation

## System Overview
Flipper Toolbox is a modular Flipper Zero application (FAP) built using the official uFBT build system and Flipper Zero firmware APIs. The application follows a modular architecture with clear separation of concerns.

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Flipper Toolbox App                      │
├─────────────────────────────────────────────────────────────┤
│  Main Entry (flipper_toolbox.c)                             │
│  ├─ ViewDispatcher (navigation, view management)            │
│  ├─ Main Menu (Submenu with 9 items)                        │
│  ├─ About Screen (Widget)                                   │
│  └─ NotificationApp (system notifications)                  │
├─────────────────────────────────────────────────────────────┤
│  Modules (loaded on demand)                                 │
│ ├─ Serial Commander          → modules/serial_commander.*   │
│ ├─ GPIO Laboratory           → modules/gpio_laboratory.*    │
│ ├─ Network Calculator        → modules/network_calculator.* │
│ ├─ Electronics Calculator → modules/electronics_calculator.*│
│ ├─ Device Profiles           → modules/device_profiles.*    │
│ ├─ Session Logs              → modules/session_logs.*       │
│ ├─ Protocol Learning         → modules/protocol_learning.*  │
│ └─ Settings                  → modules/settings.*           │
├─────────────────────────────────────────────────────────────┤
│  Shared Infrastructure                                      │
│  ├─ modules/shared.h          → Types, constants, utilities │
│  └─ modules/*.h               → Public module interfaces    │
├─────────────────────────────────────────────────────────────┤
│  Firmware APIs (Official)                                   │
│  ├─ GUI Services              → ViewDispatcher, Widgets     │
│  ├─ Storage Services          → File, Dir, Common           │
│  ├─ Furi HAL Serial           → USART, async RX/TX          │
│  ├─ Furi HAL GPIO             → Pin control, modes          │
│  ├─ Notification              → Vibro, LED, Sound           │
│  └─ Record/Event Loop         → App lifecycle               │
└─────────────────────────────────────────────────────────────┘
```

## Module Interface Pattern

Each module follows a consistent interface:

```c
// Module header (module_name.h)
typedef struct ModuleName ModuleName;
void module_name_start(ViewDispatcher* view_dispatcher, ...dependencies...);

// Module implementation (module_name.c)
typedef struct {
    ViewDispatcher* view_dispatcher;
    // Module-specific state
} ModuleView;

static void module_callbacks(...) { ... }
static bool module_nav_callback(void* context) { ... }

void module_name_start(ViewDispatcher* view_dispatcher, ...) {
    ModuleView* view = malloc(sizeof(ModuleView));
    // Initialize views, register with ViewDispatcher
    // Set navigation callback
}
```

## Memory Management

- **Static Allocation**: All buffers fixed-size (RX: 1024B, TX: 256B, display: 2048B)
- **No Dynamic Allocation in Hot Paths**: Only module view structs allocated on entry
- **Cleanup on Exit**: Navigation callback frees all allocated resources
- **Stack Size**: 4KB (defined in application.fam)

## Data Flow

### Serial Commander
```
UART Hardware → furi_hal_serial_async_rx() → Callback → Ring Buffer
                                                              ↓
User Input → TextInput → Line Ending → furi_hal_serial_tx() → UART Hardware
                                                              ↓
Logging Enabled → storage_file_write() → Internal Storage
```

### GPIO Laboratory
```
User Selection → furi_hal_resources_pin_by_number() → GpioPin*
                                                              ↓
Read:  furi_hal_gpio_read() → Display
Write: furi_hal_gpio_write() → Hardware
Cleanup: furi_hal_gpio_init(pin, GpioModeAnalog) on exit
```

### Calculators (Network/Electronics)
```
User Input → Validation → Pure C Functions → Formatted Output → Widget
                      ↓
              No floating point, integer bit ops only
```

### Persistent Storage
```
Settings/Profiles/Logs → Binary Files → /int/flipper_toolbox/
                                                              ↓
Versioned format with magic number for forward compatibility
```

## Concurrency Model

- **Single-threaded**: All code runs on main GUI thread
- **Async Serial RX**: Interrupt context callback (thread-safe ring buffer)
- **No Mutexes Needed**: No shared mutable state between modules
- **Navigation**: ViewDispatcher handles view transitions

## Error Handling Strategy

| Error Type | Handling |
|------------|----------|
| Allocation failure | Check malloc return, show error, abort module |
| Storage I/O | Check return values, show user-facing error |
| Invalid input | Validate before use, show "Error: ..." in UI |
| Hardware unavailable | Check API return, show "Error" status |
| Buffer overflow | Bounded copies (snprintf, strncpy) |

## Build Configuration

### application.fam
```python
App(
    appid="flipper_toolbox",
    name="Flipper Toolbox",
    apptype=FlipperAppType.EXTERNAL,
    entry_point="flipper_toolbox_app",
    stack_size=4 * 1024,
    fap_category="Tools",
    fap_version="0.2.0",
    fap_icon="flipper_toolbox.png",
)
```

### uFBT Build
```bash
ufbt                    # Debug build
ufbt release            # Release build
ufbt launch             # Build and launch on device
```

## Module Dependencies

| Module | Dependencies |
|--------|-------------|
| Serial Commander | Settings, Notification, Storage, HAL Serial |
| GPIO Laboratory | Notification, HAL GPIO, HAL Resources |
| Network Calculator | None (pure computation) |
| Electronics Calculator | None (pure computation) |
| Device Profiles | Settings, Storage |
| Session Logs | Storage |
| Protocol Learning | None |
| Settings | Storage |

## Extensibility Points

1. **New Modules**: Add `module_name_start()` to main menu callback
2. **Profile Types**: Extend `DeviceProfile.type` enum and union
3. **Calculator Types**: Add to `CalcType` enum and `electronics_calculator.c`
4. **Protocol Topics**: Add to `topics[]` and `topic_content[]` arrays
5. **Settings**: Add VariableItem to `settings_start()`

## Security Boundaries

- No code execution from stored data
- All file paths validated and bounded
- No network sockets (offline-only)
- UART logging opt-in only
- GPIO output requires explicit mode change + warning acknowledgment
