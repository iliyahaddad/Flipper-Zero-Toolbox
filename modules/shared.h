/**
 * Flipper Toolbox - Shared Definitions and Utilities
 */

#ifndef FLIPPER_TOOLBOX_SHARED_H
#define FLIPPER_TOOLBOX_SHARED_H

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <gui/modules/dialog_ex.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

// VariableItemList API additions
void variable_item_add_value(VariableItem* item, const char* current_value_text, const char* next_value_text);

#define STORAGE_APP_DIR "flipper_toolbox"
#define PROFILES_FILE "profiles.bin"
#define LOGS_DIR "logs"
#define SETTINGS_FILE "settings.bin"

#define MAX_BUFFER_SIZE 256
#define MAX_PROFILE_NAME 32
#define MAX_LOG_NAME 64
#define MAX_TEXT_INPUT 128

typedef enum {
    DISPLAY_MODE_ASCII = 0,
    DISPLAY_MODE_HEX = 1,
    DISPLAY_MODE_BOTH = 2,
} DisplayMode;

typedef enum {
    LINE_ENDING_NONE = 0,
    LINE_ENDING_CR = 1,
    LINE_ENDING_LF = 2,
    LINE_ENDING_CRLF = 3,
} LineEnding;

typedef struct {
    char name[MAX_PROFILE_NAME];
    uint32_t baud_rate;
    DisplayMode display_mode;
    LineEnding line_ending;
    bool logging_enabled;
    bool echo_enabled;
} SerialProfile;

typedef struct {
    char name[64];
    uint8_t pin;
    bool is_output;
    bool state;
} GpioProfile;

typedef struct {
    uint32_t ip_address;
    uint8_t cidr;
} NetworkProfile;

typedef struct {
    double voltage;
    double current;
    double resistance;
} ElectronicsProfile;

typedef struct {
    char name[MAX_PROFILE_NAME];
    uint8_t type;
    union {
        SerialProfile serial;
        GpioProfile gpio;
        NetworkProfile network;
        ElectronicsProfile electronics;
    } data;
} DeviceProfile;

typedef struct Settings Settings;

static inline void safe_strncpy(char* dest, const char* src, size_t dest_size) {
    if(!dest || !src || dest_size == 0) return;
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

static inline bool is_valid_baud_rate(uint32_t baud) {
    switch(baud) {
        case 1200: case 2400: case 4800: case 9600:
        case 19200: case 38400: case 57600: case 115200:
            return true;
        default:
            return false;
    }
}

static inline const char* baud_rate_to_str(uint32_t baud) {
    switch(baud) {
        case 1200: return "1200";
        case 2400: return "2400";
        case 4800: return "4800";
        case 9600: return "9600";
        case 19200: return "19200";
        case 38400: return "38400";
        case 57600: return "57600";
        case 115200: return "115200";
        default: return "Custom";
    }
}

static inline uint32_t str_to_baud_rate(const char* str) {
    return (uint32_t)strtoul(str, NULL, 10);
}

static inline const char* display_mode_to_str(DisplayMode mode) {
    switch(mode) {
        case DISPLAY_MODE_ASCII: return "ASCII";
        case DISPLAY_MODE_HEX: return "HEX";
        case DISPLAY_MODE_BOTH: return "ASCII+HEX";
        default: return "Unknown";
    }
}

static inline const char* line_ending_to_str(LineEnding le) {
    switch(le) {
        case LINE_ENDING_NONE: return "None";
        case LINE_ENDING_CR: return "CR";
        case LINE_ENDING_LF: return "LF";
        case LINE_ENDING_CRLF: return "CRLF";
        default: return "None";
    }
}

static inline void append_line_ending(char* buffer, size_t buffer_size, LineEnding le) {
    size_t len = strlen(buffer);
    switch(le) {
        case LINE_ENDING_CR:
            if(len + 1 < buffer_size) { buffer[len] = '\r'; buffer[len + 1] = '\0'; }
            break;
        case LINE_ENDING_LF:
            if(len + 1 < buffer_size) { buffer[len] = '\n'; buffer[len + 1] = '\0'; }
            break;
        case LINE_ENDING_CRLF:
            if(len + 2 < buffer_size) { buffer[len] = '\r'; buffer[len + 1] = '\n'; buffer[len + 2] = '\0'; }
            break;
        default:
            break;
    }
}

static inline uint32_t ip_str_to_uint(const char* ip_str) {
    unsigned int a, b, c, d;
    if(sscanf(ip_str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4) return 0;
    if(a > 255 || b > 255 || c > 255 || d > 255) return 0;
    return (a << 24) | (b << 16) | (c << 8) | d;
}

static inline void ip_uint_to_str(uint32_t ip, char* buffer, size_t size) {
    uint8_t b1 = (ip >> 24) & 0xFF;
    uint8_t b2 = (ip >> 16) & 0xFF;
    uint8_t b3 = (ip >> 8) & 0xFF;
    uint8_t b4 = ip & 0xFF;
    snprintf(buffer, size, "%u.%u.%u.%u", b1, b2, b3, b4);
}

static inline uint32_t cidr_to_mask(uint8_t cidr) {
    if(cidr == 0) return 0;
    if(cidr > 32) cidr = 32;
    return 0xFFFFFFFFu << (32 - cidr);
}

static inline uint8_t mask_to_cidr(uint32_t mask) {
    uint8_t cidr = 0;
    for(int i = 0; i < 32; i++) {
        if(mask & (1u << (31 - i))) cidr++;
        else break;
    }
    return cidr;
}

static inline bool is_private_ip(uint32_t ip) {
    uint32_t ip_10 = 0x0A000000;
    uint32_t ip_172_16 = 0xAC100000;
    uint32_t ip_172_31 = 0xAC1F0000;
    uint32_t ip_192_168 = 0xC0A80000;
    uint32_t ip_192_168_end = 0xC0A8FFFF;
    return (ip == (ip & 0xFF000000) && ip >= ip_10 && ip <= 0x0AFFFFFF) ||
           (ip >= ip_172_16 && ip <= ip_172_31) ||
           (ip >= ip_192_168 && ip <= ip_192_168_end);
}

#endif // FLIPPER_TOOLBOX_SHARED_H