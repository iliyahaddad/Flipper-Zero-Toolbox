# Contributing to Flipper Toolbox

Thank you for considering contributing to Flipper Toolbox! This guide will help you get started.

## Getting Started

### Prerequisites
- Flipper Zero device (firmware 1.5.1+)
- Python 3.10+
- uFBT build tool (`pip install ufbt`)
- Git

### Development Setup
```bash
git clone https://github.com/your-repo/flipper-toolbox.git
cd flipper-toolbox
python -m pip install ufbt
ufbt
```

## Contribution Guidelines

### Bug Reports
- Use GitHub Issues
- Include firmware version, build output, steps to reproduce
- Include hardware connections if applicable

### Feature Requests
- Open a GitHub Issue with the feature description
- Explain use case and why it's needed
- Consider hardware safety implications

### Pull Requests
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature`
3. Make your changes
4. Build and test: `ufbt release`
5. Run unit tests: `cd tests && python3 *.py`
6. Commit with descriptive message
7. Push and open a Pull Request

### Code Style
- Follow existing naming conventions (snake_case for functions/vars)
- All static functions prefixed with module name (e.g., `serial_cmd_`)
- Document functions with comment headers
- Check malloc returns, use snprintf (not sprintf)
- No dynamic allocation in hot paths (callbacks, render loops)

### Hardware Safety
- All GPIO code must include voltage warnings
- Never suggest connecting to mains voltage
- Document level converter requirements for non-3.3V devices
- Restore pin modes on module exit
