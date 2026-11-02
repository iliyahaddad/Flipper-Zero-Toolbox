# Flipper Toolbox - Development Guide

## Development Environment Setup

### Windows (PowerShell)
```powershell
# Install Python
winget install Python.Python.3.11

# Install uFBT
python -m pip install ufbt

# Verify
ufbt --version
```

### Linux/macOS
```bash
# Install Python (usually pre-installed)
# Ubuntu/Debian:
sudo apt update && sudo apt install python3 python3-pip

# macOS:
brew install python3

# Install uFBT
pip3 install ufbt

# Verify
ufbt --version
```

## Project Structure

```
flipper-toolbox/
├── application.fam          # Build manifest
├── flipper_toolbox.c        # Main entry point
├── flipper_toolbox.png      # App icon (10x10, 1-bit)
├── modules/
│   ├── shared.h             # Common types, utilities
│   ├── serial_commander.h/c
│   ├── gpio_laboratory.h/c
│   ├── network_calculator.h/c
│   ├── electronics_calculator.h/c
│   ├── device_profiles.h/c
│   ├── session_logs.h/c
│   ├── protocol_learning.h/c
│   └── settings.h/c
├── docs/                    # Documentation
├── tests/                   # Unit tests
├── scripts/                 # Build scripts
└── .github/workflows/       # CI/CD
```

## Build Commands

### Debug Build (default)
```bash
ufbt
# Output: dist/debug/flipper_toolbox_d.elf
```

### Release Build
```bash
ufbt release
# Output: dist/flipper_toolbox.fap
```

### Launch on Device
```bash
ufbt launch
# Builds debug, installs via USB, launches app
```

### Clean Build
```bash
ufbt clean
ufbt
```

### Generate Compile Commands (for IDE)
```bash
ufbt
# Creates .vscode/compile_commands.json
```

## Development Workflow

### Adding a New Module

1. **Create header** (`modules/new_module.h`):
```c
#ifndef NEW_MODULE_H
#define NEW_MODULE_H
#include "shared.h"
void new_module_start(ViewDispatcher* view_dispatcher);
#endif
```

2. **Create implementation** (`modules/new_module.c`):
```c
#include "new_module.h"
#include <furi.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>

typedef struct { ... } NewModuleView;

static bool nav_callback(void* context) { ... }
static void callbacks(...) { ... }

void new_module_start(ViewDispatcher* view_dispatcher) {
    NewModuleView* view = malloc(sizeof(NewModuleView));
    // Initialize views
    view_dispatcher_add_view(...);
    view_dispatcher_set_navigation_event_callback(view_dispatcher, nav_callback);
}
```

3. **Register in main** (`flipper_toolbox.c`):
```c
#include "modules/new_module.h"
// In main_menu_callback:
case NEW_INDEX:
    new_module_start(app->view_dispatcher);
    break;
```

4. **Add to menu** (in `flipper_toolbox_alloc()`):
```c
submenu_add_item(app->main_menu, "New Module", NEW_INDEX, main_menu_callback, app);
```

### Coding Standards

#### Naming Conventions
- **Types**: `PascalCase` (`DeviceProfile`, `NetworkCalcResult`)
- **Functions**: `snake_case` (`network_calculate`, `serial_cmd_connect`)
- **Variables**: `snake_case` (`rx_buffer`, `view_dispatcher`)
- **Constants**: `UPPER_SNAKE_CASE` (`MAX_BUFFER_SIZE`, `SERIAL_STATE_CONNECTED`)
- **Module Prefix**: All functions/statics prefixed (`serial_cmd_`, `gpio_lab_`)

#### Memory Rules
- **No malloc in hot paths** (RX callback, render loop)
- **Static buffers** for all I/O (defined in view struct)
- **Cleanup in nav_callback** - always free on exit
- **Check malloc returns** - handle allocation failure

#### Error Handling
```c
// Always validate
if(!ptr) return false;
if(size > MAX) return false;

// Use snprintf, not sprintf
snprintf(buf, sizeof(buf), "format", val);

// Check storage returns
if(!storage_file_open(...)) { show_error(); return; }
```

#### UI Guidelines
- **Widget text**: Use `widget_add_text_box_element()` for multi-line
- **Single line**: `widget_add_string_element()`
- **Buttons**: `widget_add_button_element(widget, GuiButtonTypeRight, "Label", callback, context)`
- **Navigation**: Always provide Back button (Left or navigation callback)

## Debugging

### On Device (via USB)
```bash
# View logs
ufbt log

# GDB Debug (requires debug build)
ufbt gdb
```

### Common Issues

| Issue | Solution |
|-------|----------|
| App not in menu | Check `application.fam` apptype=EXTERNAL |
| Crash on launch | Check `flipper_toolbox_app` return value |
| Serial not working | Verify `furi_hal_serial_control_acquire` succeeds |
| GPIO not responding | Check `furi_hal_resources_pin_by_number` returns valid pin |
| Storage fails | Verify path uses `INT_PATH("")` macro |

## Testing

### Unit Tests (Calculator Logic)
```bash
cd tests
python3 test_network_calculator.py
python3 test_electronics_calculator.py
```

### Manual Testing Checklist
- [ ] App launches from Tools menu
- [ ] All 9 menu items navigate correctly
- [ ] Back button returns to main menu
- [ ] Serial Commander: connect, send, receive, log
- [ ] GPIO Lab: warning, pin select, read, write, toggle, pattern
- [ ] Network Calc: IP/CIDR input, correct results
- [ ] Electronics Calc: all 11 types, correct results
- [ ] Profiles: create, view, delete, persist
- [ ] Logs: list, view, delete with confirmation
- [ ] Protocol Learning: all 7 topics display
- [ ] Settings: all 4 options persist

## Code Style

### Header Guards
```c
#ifndef MODULE_NAME_H
#define MODULE_NAME_H
// ...
#endif // MODULE_NAME_H
```

### Includes Order
```c
#include <furi.h>           // System
#include <gui/gui.h>        // GUI
#include <storage/storage.h> // Services
#include "shared.h"         // Project shared
#include "module_name.h"    // Self
```

### Function Documentation
```c
/**
 * Brief description.
 * 
 * @param param1 Description
 * @return Description
 */
ReturnType function_name(ParamType param1) { ... }
```

## Profiling & Optimization

### Stack Usage
- Monitor via `furi_get_stack_usage()` if available
- Current: 4KB allocated, typically <2KB used

### Binary Size
- Debug ELF: ~195KB
- Release FAP: ~45KB
- Target: <64KB for FAP

### Performance
- No dynamic allocation in callbacks
- Ring buffer for serial RX (no copy)
- Widget text updates only on change

## Releasing

### Version Bump
1. Update `fap_version` in `application.fam`
2. Update `CHANGELOG.md`
3. Tag release: `git tag v0.2.0`

### Build Release
```bash
ufbt release
# Verify dist/flipper_toolbox.fap
```

### GitHub Release
1. Create release on GitHub
2. Upload `flipper_toolbox.fap`
3. Copy CHANGELOG entry to release notes