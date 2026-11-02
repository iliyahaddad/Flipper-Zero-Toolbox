/**
 * Flipper Toolbox - Settings Module Implementation
 */

#include "settings.h"
#include <furi.h>
#include <storage/storage.h>

#define SETTINGS_VERSION 1

struct Settings {
    uint32_t default_baud;
    DisplayMode default_display_mode;
    LineEnding default_line_ending;
    bool confirm_on_delete;
    bool loaded;
};

Settings* settings_alloc(void) {
    Settings* settings = malloc(sizeof(Settings));
    settings->default_baud = 115200;
    settings->default_display_mode = DISPLAY_MODE_ASCII;
    settings->default_line_ending = LINE_ENDING_CRLF;
    settings->confirm_on_delete = true;
    settings->loaded = false;
    return settings;
}

void settings_free(Settings* settings) {
    if(settings) free(settings);
}

static char* settings_get_path(void) {
    const char* base_path = INT_PATH("");
    if(!base_path) return NULL;
    
    char* path = malloc(strlen(base_path) + 1 + strlen(STORAGE_APP_DIR) + 1 + strlen(SETTINGS_FILE) + 1);
    if(path) {
        sprintf(path, "%s/%s/%s", base_path, STORAGE_APP_DIR, SETTINGS_FILE);
    }
    return path;
}

bool settings_load(Settings* settings) {
    if(!settings) return false;
    
    char* path = settings_get_path();
    if(!path) return false;
    
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    
    bool success = false;
    if(storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint32_t version = 0;
        if(storage_file_read(file, &version, sizeof(version)) == sizeof(version)) {
            if(version == SETTINGS_VERSION) {
                if(storage_file_read(file, &settings->default_baud, sizeof(settings->default_baud)) == sizeof(settings->default_baud) &&
                   storage_file_read(file, &settings->default_display_mode, sizeof(settings->default_display_mode)) == sizeof(settings->default_display_mode) &&
                   storage_file_read(file, &settings->default_line_ending, sizeof(settings->default_line_ending)) == sizeof(settings->default_line_ending) &&
                   storage_file_read(file, &settings->confirm_on_delete, sizeof(settings->confirm_on_delete)) == sizeof(settings->confirm_on_delete)) {
                    settings->loaded = true;
                    success = true;
                }
            }
        }
        storage_file_close(file);
    }
    
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    free(path);
    return success;
}

bool settings_save(Settings* settings) {
    if(!settings) return false;
    
    char* path = settings_get_path();
    if(!path) return false;
    
    Storage* storage = furi_record_open(RECORD_STORAGE);
    
    char* dir_path = settings_get_path();
    if(dir_path) {
        char* last_slash = strrchr(dir_path, '/');
        if(last_slash) {
            *last_slash = '\0';
            storage_simply_mkdir(storage, dir_path);
            *last_slash = '/';
        }
        free(dir_path);
    }
    
    File* file = storage_file_alloc(storage);
    bool success = false;
    
    if(storage_file_open(file, path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        uint32_t version = SETTINGS_VERSION;
        if(storage_file_write(file, &version, sizeof(version)) == sizeof(version) &&
           storage_file_write(file, &settings->default_baud, sizeof(settings->default_baud)) == sizeof(settings->default_baud) &&
           storage_file_write(file, &settings->default_display_mode, sizeof(settings->default_display_mode)) == sizeof(settings->default_display_mode) &&
           storage_file_write(file, &settings->default_line_ending, sizeof(settings->default_line_ending)) == sizeof(settings->default_line_ending) &&
           storage_file_write(file, &settings->confirm_on_delete, sizeof(settings->confirm_on_delete)) == sizeof(settings->confirm_on_delete)) {
            success = true;
        }
        storage_file_close(file);
    }
    
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    free(path);
    return success;
}

uint32_t settings_get_default_baud(Settings* settings) {
    return settings ? settings->default_baud : 115200;
}

void settings_set_default_baud(Settings* settings, uint32_t baud) {
    if(settings && is_valid_baud_rate(baud)) {
        settings->default_baud = baud;
        settings_save(settings);
    }
}

DisplayMode settings_get_default_display_mode(Settings* settings) {
    return settings ? settings->default_display_mode : DISPLAY_MODE_ASCII;
}

void settings_set_default_display_mode(Settings* settings, DisplayMode mode) {
    if(settings && mode <= DISPLAY_MODE_BOTH) {
        settings->default_display_mode = mode;
        settings_save(settings);
    }
}

LineEnding settings_get_default_line_ending(Settings* settings) {
    return settings ? settings->default_line_ending : LINE_ENDING_CRLF;
}

void settings_set_default_line_ending(Settings* settings, LineEnding le) {
    if(settings && le <= LINE_ENDING_CRLF) {
        settings->default_line_ending = le;
        settings_save(settings);
    }
}

bool settings_get_confirm_on_delete(Settings* settings) {
    return settings ? settings->confirm_on_delete : true;
}

void settings_set_confirm_on_delete(Settings* settings, bool confirm) {
    if(settings) {
        settings->confirm_on_delete = confirm;
        settings_save(settings);
    }
}

// Settings UI Implementation

typedef struct {
    Settings* settings;
    VariableItemList* var_list;
    ViewDispatcher* view_dispatcher;
} SettingsView;

static void settings_var_item_baud_callback(VariableItem* item) {
    SettingsView* view = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint32_t baud_rates[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
    settings_set_default_baud(view->settings, baud_rates[index]);
    variable_item_set_current_value_text(item, baud_rate_to_str(baud_rates[index]));
}

static void settings_var_item_display_callback(VariableItem* item) {
    SettingsView* view = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    settings_set_default_display_mode(view->settings, (DisplayMode)index);
    variable_item_set_current_value_text(item, display_mode_to_str((DisplayMode)index));
}

static void settings_var_item_line_ending_callback(VariableItem* item) {
    SettingsView* view = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    settings_set_default_line_ending(view->settings, (LineEnding)index);
    variable_item_set_current_value_text(item, line_ending_to_str((LineEnding)index));
}

static void settings_var_item_confirm_callback(VariableItem* item) {
    SettingsView* view = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    settings_set_confirm_on_delete(view->settings, index == 0);
    variable_item_set_current_value_text(item, index == 0 ? "Yes" : "No");
}

static bool settings_back_callback(void* context) {
    SettingsView* view = context;
    view_dispatcher_stop(view->view_dispatcher);
    variable_item_list_free(view->var_list);
    free(view);
    return true;
}

void settings_start(ViewDispatcher* view_dispatcher, Settings* settings) {
    SettingsView* view = malloc(sizeof(SettingsView));
    view->settings = settings;
    view->view_dispatcher = view_dispatcher;
    
    view->var_list = variable_item_list_alloc();
    
    VariableItem* item;
    
    item = variable_item_list_add(view->var_list, "Default Baud", 8, settings_var_item_baud_callback, view);
    uint32_t baud_rates[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
    for(int i = 0; i < 8; i++) {
        variable_item_add_value(item, baud_rate_to_str(baud_rates[i]), NULL);
    }
    for(int i = 0; i < 8; i++) {
        if(baud_rates[i] == settings->default_baud) {
            variable_item_set_current_value_index(item, i);
            break;
        }
    }
    
    item = variable_item_list_add(view->var_list, "Display Mode", 3, settings_var_item_display_callback, view);
    variable_item_add_value(item, "ASCII", NULL);
    variable_item_add_value(item, "HEX", NULL);
    variable_item_add_value(item, "ASCII+HEX", NULL);
    variable_item_set_current_value_index(item, settings->default_display_mode);
    
    item = variable_item_list_add(view->var_list, "Line Ending", 4, settings_var_item_line_ending_callback, view);
    variable_item_add_value(item, "None", NULL);
    variable_item_add_value(item, "CR", NULL);
    variable_item_add_value(item, "LF", NULL);
    variable_item_add_value(item, "CRLF", NULL);
    variable_item_set_current_value_index(item, settings->default_line_ending);
    
    item = variable_item_list_add(view->var_list, "Confirm Delete", 2, settings_var_item_confirm_callback, view);
    variable_item_add_value(item, "Yes", NULL);
    variable_item_add_value(item, "No", NULL);
    variable_item_set_current_value_index(item, settings->confirm_on_delete ? 0 : 1);
    
    view_dispatcher_add_view(view_dispatcher, 100, variable_item_list_get_view(view->var_list));
    view_dispatcher_switch_to_view(view_dispatcher, 100);
    
    view_dispatcher_set_navigation_event_callback(view_dispatcher, settings_back_callback);
}