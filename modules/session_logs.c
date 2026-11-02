/**
 * Flipper Toolbox - Session Logs Module Implementation
 */

#include "session_logs.h"
#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>

#define TAG "SessionLogs"

#define MAX_LOGS 32
#define MAX_LOG_PATH 256

typedef struct {
    VariableItemList* var_list;
    ViewDispatcher* view_dispatcher;
    Widget* log_widget;
    Widget* confirm_widget;
    
    char log_files[MAX_LOGS][MAX_LOG_PATH];
    uint8_t log_count;
    int8_t selected_log;
    bool show_confirm;
} SessionLogsView;

static void session_logs_view_callback(VariableItem* item);

static void session_logs_scan(SessionLogsView* view) {
    view->log_count = 0;
    
    Storage* storage = furi_record_open(RECORD_STORAGE);
    const char* base_path = INT_PATH("");
    if(!base_path) {
        furi_record_close(RECORD_STORAGE);
        return;
    }
    
    char logs_path[128];
    snprintf(logs_path, sizeof(logs_path), "%s/%s/%s", base_path, STORAGE_APP_DIR, LOGS_DIR);
    
    File* dir = storage_file_alloc(storage);
    if(storage_dir_open(dir, logs_path)) {
        FileInfo file_info;
        char name[64];
        while(storage_dir_read(dir, &file_info, name, sizeof(name)) && view->log_count < MAX_LOGS) {
            if(!file_info_is_dir(&file_info)) {
                snprintf(view->log_files[view->log_count], MAX_LOG_PATH, "%s/%s", logs_path, name);
                view->log_count++;
            }
        }
        storage_dir_close(dir);
    }
    storage_file_free(dir);
    furi_record_close(RECORD_STORAGE);
}

static void session_logs_update_list(SessionLogsView* view) {
    variable_item_list_reset(view->var_list);
    
    for(uint8_t i = 0; i < view->log_count; i++) {
        const char* filename = strrchr(view->log_files[i], '/');
        filename = filename ? filename + 1 : view->log_files[i];
        
        VariableItem* item = variable_item_list_add(view->var_list, filename, 0, session_logs_view_callback, view);
        variable_item_set_current_value_text(item, "View");
    }
    
    if(view->log_count == 0) {
        VariableItem* item = variable_item_list_add(view->var_list, "No logs found", 0, NULL, view);
        variable_item_set_current_value_text(item, "");
    }
}

static void session_logs_view_callback(VariableItem* item) {
    SessionLogsView* view = variable_item_get_context(item);
    view->selected_log = variable_item_get_current_value_index(item);
    
    if(view->selected_log >= 0 && view->selected_log < view->log_count) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        File* file = storage_file_alloc(storage);
        
        if(storage_file_open(file, view->log_files[view->selected_log], FSAM_READ, FSOM_OPEN_EXISTING)) {
            size_t size = storage_file_size(file);
            size_t read_size = size < 2048 ? size : 2048;
            char* buffer = malloc(read_size + 1);
            
            if(buffer) {
                storage_file_read(file, buffer, read_size);
                buffer[read_size] = '\0';
                
                char header[128];
                const char* filename = strrchr(view->log_files[view->selected_log], '/');
                filename = filename ? filename + 1 : view->log_files[view->selected_log];
                snprintf(header, sizeof(header), "Log: %s (%zu bytes)\n---\n", filename, size);
                
                char* full_buffer = malloc(strlen(header) + read_size + 1);
                if(full_buffer) {
                    strcpy(full_buffer, header);
                    strcat(full_buffer, buffer);
                    widget_reset(view->log_widget);
                    widget_add_text_box_element(view->log_widget, 4, 4, 120, 56, AlignLeft, AlignTop, full_buffer, false);
                    free(full_buffer);
                }
                free(buffer);
            }
            storage_file_close(file);
        }
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        
        view_dispatcher_switch_to_view(view->view_dispatcher, 1);
    }
}

static void session_logs_delete_confirm_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    SessionLogsView* view = context;
    if(view->show_confirm) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        storage_common_remove(storage, view->log_files[view->selected_log]);
        furi_record_close(RECORD_STORAGE);
        
        session_logs_scan(view);
        session_logs_update_list(view);
        view->show_confirm = false;
        view_dispatcher_switch_to_view(view->view_dispatcher, 0);
    } else {
        view->show_confirm = true;
        widget_reset(view->confirm_widget);
        widget_add_string_element(view->confirm_widget, 4, 4, AlignLeft, AlignTop, FontSecondary, "Delete this log?\nPress OK to confirm\nPress Back to cancel");
        view_dispatcher_switch_to_view(view->view_dispatcher, 2);
    }
}

static void log_back_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    SessionLogsView* view = context;
    view_dispatcher_switch_to_view(view->view_dispatcher, 0);
}

static void confirm_cancel_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    SessionLogsView* view = context;
    view->show_confirm = false;
    view_dispatcher_switch_to_view(view->view_dispatcher, 0);
}

static bool session_logs_nav_callback(void* context) {
    SessionLogsView* view = context;
    if(view->show_confirm) {
        view->show_confirm = false;
        view_dispatcher_switch_to_view(view->view_dispatcher, 0);
    } else {
        view_dispatcher_stop(view->view_dispatcher);
        variable_item_list_free(view->var_list);
        widget_free(view->log_widget);
        widget_free(view->confirm_widget);
        free(view);
    }
    return true;
}

void session_logs_start(ViewDispatcher* view_dispatcher) {
    SessionLogsView* view = malloc(sizeof(SessionLogsView));
    view->view_dispatcher = view_dispatcher;
    view->log_count = 0;
    view->selected_log = -1;
    view->show_confirm = false;
    
    session_logs_scan(view);
    
    view->var_list = variable_item_list_alloc();
    session_logs_update_list(view);
    
    view->log_widget = widget_alloc();
    widget_add_button_element(view->log_widget, GuiButtonTypeLeft, "Back", log_back_callback, view);
    
    view->confirm_widget = widget_alloc();
    widget_add_button_element(view->confirm_widget, GuiButtonTypeRight, "OK", session_logs_delete_confirm_callback, view);
    widget_add_button_element(view->confirm_widget, GuiButtonTypeLeft, "Cancel", confirm_cancel_callback, view);
    
    view_dispatcher_add_view(view_dispatcher, 0, variable_item_list_get_view(view->var_list));
    view_dispatcher_add_view(view_dispatcher, 1, widget_get_view(view->log_widget));
    view_dispatcher_add_view(view_dispatcher, 2, widget_get_view(view->confirm_widget));
    view_dispatcher_switch_to_view(view_dispatcher, 0);
    
    view_dispatcher_set_navigation_event_callback(view_dispatcher, session_logs_nav_callback);
}