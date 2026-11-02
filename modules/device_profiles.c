/**
 * Flipper Toolbox - Device Profiles Module Implementation
 */

#include "device_profiles.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <gui/modules/text_input.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>

#define TAG "Profiles"

#define MAX_PROFILES 16

typedef struct {
    VariableItemList* var_list;
    ViewDispatcher* view_dispatcher;
    Widget* detail_widget;
    Widget* edit_widget;
    TextInput* text_input;
    Settings* settings;
    
    DeviceProfile profiles[MAX_PROFILES];
    uint8_t profile_count;
    int8_t selected_profile;
    char edit_buffer[64];
} ProfilesView;

// Forward declarations
static char* profiles_get_path(void);
static bool profiles_load(ProfilesView* view);
static bool profiles_save(ProfilesView* view);
static void profiles_update_list(ProfilesView* view);
static void profiles_add_item_callback(VariableItem* item);
static void profiles_select_callback(VariableItem* item);
static void profiles_add_button_callback(GuiButtonType type, InputType event, void* context);
static void profiles_delete_callback(GuiButtonType type, InputType event, void* context);
static bool profiles_back_callback(void* context);
static void detail_back_callback(GuiButtonType type, InputType event, void* context);
static void edit_back_callback(GuiButtonType type, InputType event, void* context);

static char* profiles_get_path(void) {
    const char* base_path = INT_PATH("");
    if(!base_path) return NULL;
    
    char* path = malloc(strlen(base_path) + 1 + strlen(STORAGE_APP_DIR) + 1 + strlen(PROFILES_FILE) + 1);
    if(path) {
        sprintf(path, "%s/%s/%s", base_path, STORAGE_APP_DIR, PROFILES_FILE);
    }
    return path;
}

static bool profiles_load(ProfilesView* view) {
    char* path = profiles_get_path();
    if(!path) return false;
    
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    
    bool success = false;
    if(storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint32_t count = 0;
        if(storage_file_read(file, &count, sizeof(count)) == sizeof(count)) {
            if(count <= MAX_PROFILES) {
                view->profile_count = (uint8_t)count;
                for(uint8_t i = 0; i < view->profile_count; i++) {
                    if(storage_file_read(file, &view->profiles[i], sizeof(DeviceProfile)) != sizeof(DeviceProfile)) {
                        view->profile_count = i;
                        break;
                    }
                }
                success = true;
            }
        }
        storage_file_close(file);
    }
    
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    free(path);
    return success;
}

static bool profiles_save(ProfilesView* view) {
    char* path = profiles_get_path();
    if(!path) return false;
    
    Storage* storage = furi_record_open(RECORD_STORAGE);
    
    char* dir_path = profiles_get_path();
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
        uint32_t count = view->profile_count;
        if(storage_file_write(file, &count, sizeof(count)) == sizeof(count)) {
            for(uint8_t i = 0; i < view->profile_count; i++) {
                if(storage_file_write(file, &view->profiles[i], sizeof(DeviceProfile)) != sizeof(DeviceProfile)) {
                    break;
                }
            }
            success = true;
        }
        storage_file_close(file);
    }
    
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    free(path);
    return success;
}

static void profiles_update_list(ProfilesView* view) {
    variable_item_list_reset(view->var_list);
    
    for(uint8_t i = 0; i < view->profile_count; i++) {
        VariableItem* item = variable_item_list_add(view->var_list, view->profiles[i].name, 0, profiles_select_callback, view);
        variable_item_set_current_value_text(item, "Select");
    }
    
    VariableItem* item = variable_item_list_add(view->var_list, "Add New Profile", 0, profiles_add_item_callback, view);
    variable_item_set_current_value_text(item, "Press OK");
}

static void profiles_add_item_callback(VariableItem* item) {
    ProfilesView* view = variable_item_get_context(item);
    if(view->profile_count >= MAX_PROFILES) return;
    
    view->selected_profile = view->profile_count;
    memset(&view->profiles[view->selected_profile], 0, sizeof(DeviceProfile));
    strcpy(view->profiles[view->selected_profile].name, "New Profile");
    view->profiles[view->selected_profile].type = 0;
    strcpy(view->edit_buffer, "New Profile");
    
    text_input_set_header_text(view->text_input, "Profile Name");
    text_input_set_result_callback(view->text_input, NULL, view, view->edit_buffer, sizeof(view->edit_buffer), false);
    view_dispatcher_switch_to_view(view->view_dispatcher, 2);
}

static void profiles_select_callback(VariableItem* item) {
    ProfilesView* view = variable_item_get_context(item);
    view->selected_profile = variable_item_get_current_value_index(item);
    if(view->selected_profile >= 0 && view->selected_profile < view->profile_count) {
        DeviceProfile* p = &view->profiles[view->selected_profile];
        char buffer[512];
        
        snprintf(buffer, sizeof(buffer),
            "Profile: %s\nType: %s\n",
            p->name, p->type == 0 ? "Serial" : p->type == 1 ? "GPIO" : p->type == 2 ? "Network" : "Electronics");
        
        if(p->type == 0) {
            snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
                "Baud: %lu\nDisplay: %s\nLine Ending: %s\nLogging: %s",
                p->data.serial.baud_rate,
                display_mode_to_str(p->data.serial.display_mode),
                line_ending_to_str(p->data.serial.line_ending),
                p->data.serial.logging_enabled ? "On" : "Off");
        }
        
        widget_reset(view->detail_widget);
        widget_add_text_box_element(view->detail_widget, 4, 4, 120, 56, AlignLeft, AlignTop, buffer, false);
        view_dispatcher_switch_to_view(view->view_dispatcher, 1);
    }
}

static void profiles_add_button_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    ProfilesView* view = context;
    if(view->profile_count >= MAX_PROFILES) return;
    
    view->selected_profile = view->profile_count;
    memset(&view->profiles[view->selected_profile], 0, sizeof(DeviceProfile));
    strcpy(view->profiles[view->selected_profile].name, "New Profile");
    view->profiles[view->selected_profile].type = 0;
    strcpy(view->edit_buffer, "New Profile");
    
    text_input_set_header_text(view->text_input, "Profile Name");
    text_input_set_result_callback(view->text_input, NULL, view, view->edit_buffer, sizeof(view->edit_buffer), false);
    view_dispatcher_switch_to_view(view->view_dispatcher, 2);
}

static void profiles_delete_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    ProfilesView* view = context;
    if(view->selected_profile >= 0 && view->selected_profile < view->profile_count) {
        for(int i = view->selected_profile; i < view->profile_count - 1; i++) {
            view->profiles[i] = view->profiles[i + 1];
        }
        view->profile_count--;
        profiles_save(view);
        profiles_update_list(view);
        view_dispatcher_switch_to_view(view->view_dispatcher, 0);
    }
}

static bool profiles_back_callback(void* context) {
    ProfilesView* view = context;
    view_dispatcher_stop(view->view_dispatcher);
    variable_item_list_free(view->var_list);
    widget_free(view->detail_widget);
    widget_free(view->edit_widget);
    text_input_free(view->text_input);
    free(view);
    return true;
}

static void detail_back_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    ProfilesView* view = context;
    view_dispatcher_switch_to_view(view->view_dispatcher, 0);
}

static void edit_back_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    ProfilesView* view = context;
    view_dispatcher_switch_to_view(view->view_dispatcher, 0);
}

void device_profiles_start(ViewDispatcher* view_dispatcher, Settings* settings) {
    ProfilesView* view = malloc(sizeof(ProfilesView));
    view->view_dispatcher = view_dispatcher;
    view->settings = settings;
    view->profile_count = 0;
    view->selected_profile = -1;
    
    profiles_load(view);
    
    view->var_list = variable_item_list_alloc();
    profiles_update_list(view);
    
    view->detail_widget = widget_alloc();
    widget_add_button_element(view->detail_widget, GuiButtonTypeRight, "Delete", profiles_delete_callback, view);
    widget_add_button_element(view->detail_widget, GuiButtonTypeLeft, "Back", detail_back_callback, view);
    
    view->edit_widget = widget_alloc();
    widget_add_string_element(view->edit_widget, 64, 20, AlignCenter, AlignTop, FontPrimary, "Enter Name");
    widget_add_button_element(view->edit_widget, GuiButtonTypeRight, "OK", profiles_add_button_callback, view);
    widget_add_button_element(view->edit_widget, GuiButtonTypeLeft, "Cancel", edit_back_callback, view);
    
    view->text_input = text_input_alloc();
    
    view_dispatcher_add_view(view_dispatcher, 0, variable_item_list_get_view(view->var_list));
    view_dispatcher_add_view(view_dispatcher, 1, widget_get_view(view->detail_widget));
    view_dispatcher_add_view(view_dispatcher, 2, widget_get_view(view->edit_widget));
    view_dispatcher_switch_to_view(view_dispatcher, 0);
    
    view_dispatcher_set_navigation_event_callback(view_dispatcher, profiles_back_callback);
}