# Flipper Toolbox - Security (Root)

## Security Overview

Flipper Toolbox is designed for authorized diagnostics, electronics development, and networking education. It does not implement any unauthorized access, credential theft, replay attacks, or access-control bypass functionality.

## Key Security Properties

1. **Offline-only**: No network sockets, no Wi-Fi, no internet access
2. **No secret storage**: Profiles store configuration only, never passwords
3. **Opt-in logging**: Session data only stored when user explicitly enables
4. **Input validation**: All inputs bounded and validated
5. **Hardware safety**: Voltage warnings, pin restoration, no mains voltage

## Threat Model

| Threat | Mitigation |
|--------|------------|
| Malicious profile execution | Profiles are data only, never executed |
| Path traversal | Filename sanitization, bounded buffers |
| Buffer overflow | Fixed-size buffers, snprintf everywhere |
| Electrical damage | 3.3V warnings, level shifter documentation |
| Sensitive data leakage | Logging disabled by default, user must enable |

## Reporting Security Issues

See docs/SECURITY.md for disclosure process.