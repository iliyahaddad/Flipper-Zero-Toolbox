/**
 * Flipper Toolbox - Serial Commander Module Implementation
 * 
 * Provides serial communication over Flipper Zero's GPIO pins.
 * Note: Requires appropriate level shifting for RS-232 devices.
 */

#include "serial_commander.h"
#include "settings.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_serial.h>
#include <furi_hal_serial_control.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <gui/modules/text_input.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>

#define TAG "SerialCmd"

#define SERIAL_RX_BUFFER_SIZE 1024
#define SERIAL_TX_BUFFER_SIZE 256

typedef enum {
    SERIAL_STATE_DISCONNECTED = 0,
    SERIAL_STATE_CONNECTING = 1,
    SERIAL_STATE_CONNECTED = 2,
    SERIAL_STATE_ERROR = 3,
} SerialState;

typedef struct {
    VariableItemList* var_list;
    ViewDispatcher* view_dispatcher;
    Widget* terminal_widget;
    Widget* status_widget;
    TextInput* text_input;
    NotificationApp* notifications;
    Settings* settings;
    
    FuriHalSerialHandle* serial_handle;
    SerialState state;
    uint32_t baud_rate;
    DisplayMode display_mode;
    LineEnding line_ending;
    bool logging_enabled;
    
    char rx_buffer[SERIAL_RX_BUFFER_SIZE];
    size_t rx_buffer_len;
    char tx_buffer[SERIAL_TX_BUFFER_SIZE];
    size_t tx_buffer_len;
    
    File* log_file;
    char log_path[128];
} SerialCommanderView;

static void serial_cmd_rx_callback(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    SerialCommanderView* view = context;
    if(event == FuriHalSerialRxEventData) {
        while(furi_hal_serial_async_rx_available(handle)) {
            uint8_t data = furi_hal_serial_async_rx(handle);
            if(view->rx_buffer_len < SERIAL_RX_BUFFER_SIZE - 1) {
                view->rx_buffer[view->rx_buffer_len++] = (char)data;
                view->rx_buffer[view->rx_buffer_len] = '\0';
            }
        }
    }
}

static bool serial_cmd_connect(SerialCommanderView* view) {
    if(view->state == SERIAL_STATE_CONNECTED) return true;
    
    view->state = SERIAL_STATE_CONNECTING;
    
    view->serial_handle = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    if(!view->serial_handle) {
        view->state = SERIAL_STATE_ERROR;
        return false;
    }
    
    furi_hal_serial_init(view->serial_handle, view->baud_rate);
    furi_hal_serial_async_rx_start(view->serial_handle, serial_cmd_rx_callback, view, false);
    
    view->state = SERIAL_STATE_CONNECTED;
    return true;
}

static void serial_cmd_disconnect(SerialCommanderView* view) {
    if(view->serial_handle) {
        furi_hal_serial_async_rx_stop(view->serial_handle);
        furi_hal_serial_control_release(view->serial_handle);
        view->serial_handle = NULL;
    }
    view->state = SERIAL_STATE_DISCONNECTED;
}

static void serial_cmd_send(SerialCommanderView* view, const char* data, size_t len) {
    if(view->state != SERIAL_STATE_CONNECTED || !view->serial_handle) return;
    
    furi_hal_serial_tx(view->serial_handle, (uint8_t*)data, len);
    
    if(view->logging_enabled && view->log_file) {
        storage_file_write(view->log_file, data, len);
    }
}

static void serial_cmd_update_display(SerialCommanderView* view) {
    if(!view->terminal_widget) return;
    
    char display_buffer[2048];
    size_t display_len = 0;
    
    if(view->display_mode == DISPLAY_MODE_ASCII || view->display_mode == DISPLAY_MODE_BOTH) {
        display_len = snprintf(display_buffer, sizeof(display_buffer), "%s", view->rx_buffer);
    }
    
    if(view->display_mode == DISPLAY_MODE_HEX || view->display_mode == DISPLAY_MODE_BOTH) {
        if(view->display_mode == DISPLAY_MODE_BOTH) {
            display_len += snprintf(display_buffer + display_len, sizeof(display_buffer) - display_len, "\n--- HEX ---\n");
        }
        for(size_t i = 0; i < view->rx_buffer_len && display_len < sizeof(display_buffer) - 4; i++) {
            display_len += snprintf(display_buffer + display_len, sizeof(display_buffer) - display_len, "%02X ", (uint8_t)view->rx_buffer[i]);
        }
    }
    
    widget_reset(view->terminal_widget);
    widget_add_text_box_element(view->terminal_widget, 4, 4, 120, 56, AlignLeft, AlignTop, display_buffer, false);
}

static void serial_cmd_var_baud_changed(VariableItem* item) {
    SerialCommanderView* view = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint32_t baud_rates[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
    view->baud_rate = baud_rates[index];
    if(view->state == SERIAL_STATE_CONNECTED) {
        serial_cmd_disconnect(view);
        serial_cmd_connect(view);
    }
    variable_item_set_current_value_text(item, baud_rate_to_str(baud_rates[index]));
}

static void serial_cmd_var_mode_changed(VariableItem* item) {
    SerialCommanderView* view = variable_item_get_context(item);
    view->display_mode = (DisplayMode)variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, display_mode_to_str(view->display_mode));
}

static void serial_cmd_var_line_changed(VariableItem* item) {
    SerialCommanderView* view = variable_item_get_context(item);
    view->line_ending = (LineEnding)variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, line_ending_to_str(view->line_ending));
}

static void serial_cmd_var_logging_changed(VariableItem* item) {
    SerialCommanderView* view = variable_item_get_context(item);
    view->logging_enabled = variable_item_get_current_value_index(item) == 0;
    variable_item_set_current_value_text(item, view->logging_enabled ? "Enabled" : "Disabled");
}

static void serial_cmd_connect_callback(VariableItem* item) {
    SerialCommanderView* view = variable_item_get_context(item);
    if(view->state == SERIAL_STATE_CONNECTED) {
        serial_cmd_disconnect(view);
        variable_item_set_current_value_text(item, "Disconnected");
    } else {
        if(serial_cmd_connect(view)) {
            variable_item_set_current_value_text(item, "Connected");
            notification_message(view->notifications, &sequence_success);
        } else {
            variable_item_set_current_value_text(item, "Error");
            notification_message(view->notifications, &sequence_error);
        }
    }
}

static void serial_cmd_clear_callback(VariableItem* item) {
    SerialCommanderView* view = variable_item_get_context(item);
    view->rx_buffer_len = 0;
    view->rx_buffer[0] = '\0';
    serial_cmd_update_display(view);
}

static void serial_cmd_send_callback(void* context) {
    SerialCommanderView* view = context;
    append_line_ending(view->tx_buffer, sizeof(view->tx_buffer), view->line_ending);
    serial_cmd_send(view, view->tx_buffer, strlen(view->tx_buffer));
    view->tx_buffer[0] = '\0';
}

static void serial_cmd_send_button_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    SerialCommanderView* view = context;
    append_line_ending(view->tx_buffer, sizeof(view->tx_buffer), view->line_ending);
    serial_cmd_send(view, view->tx_buffer, strlen(view->tx_buffer));
    view->tx_buffer[0] = '\0';
}

static void serial_cmd_back_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    SerialCommanderView* view = context;
    serial_cmd_disconnect(view);
    if(view->log_file) {
        storage_file_close(view->log_file);
        storage_file_free(view->log_file);
        view->log_file = NULL;
    }
    view_dispatcher_stop(view->view_dispatcher);
    variable_item_list_free(view->var_list);
    widget_free(view->terminal_widget);
    widget_free(view->status_widget);
    text_input_free(view->text_input);
    free(view);
}

static void status_back_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    SerialCommanderView* view = context;
    view_dispatcher_switch_to_view(view->view_dispatcher, 0);
}

static bool serial_cmd_nav_callback(void* context) {
    SerialCommanderView* view = context;
    serial_cmd_disconnect(view);
    if(view->log_file) {
        storage_file_close(view->log_file);
        storage_file_free(view->log_file);
        view->log_file = NULL;
    }
    view_dispatcher_stop(view->view_dispatcher);
    variable_item_list_free(view->var_list);
    widget_free(view->terminal_widget);
    widget_free(view->status_widget);
    text_input_free(view->text_input);
    free(view);
    return true;
}

void serial_commander_start(ViewDispatcher* view_dispatcher, NotificationApp* notifications, Settings* settings) {
    SerialCommanderView* view = malloc(sizeof(SerialCommanderView));
    view->view_dispatcher = view_dispatcher;
    view->notifications = notifications;
    view->settings = settings;
    view->state = SERIAL_STATE_DISCONNECTED;
    view->baud_rate = settings_get_default_baud(settings);
    view->display_mode = settings_get_default_display_mode(settings);
    view->line_ending = settings_get_default_line_ending(settings);
    view->logging_enabled = false;
    view->rx_buffer_len = 0;
    view->rx_buffer[0] = '\0';
    view->log_file = NULL;
    view->log_path[0] = '\0';
    
    view->var_list = variable_item_list_alloc();
    
    VariableItem* item = variable_item_list_add(view->var_list, "Status", 0, serial_cmd_connect_callback, view);
    variable_item_set_current_value_text(item, "Disconnected");
    
    item = variable_item_list_add(view->var_list, "Baud Rate", 8, serial_cmd_var_baud_changed, view);
    uint32_t baud_rates[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
    for(int i = 0; i < 8; i++) {
        variable_item_add_value(item, baud_rate_to_str(baud_rates[i]), NULL);
    }
    for(int i = 0; i < 8; i++) {
        if(baud_rates[i] == view->baud_rate) {
            variable_item_set_current_value_index(item, i);
            break;
        }
    }
    
    item = variable_item_list_add(view->var_list, "Display", 3, serial_cmd_var_mode_changed, view);
    variable_item_add_value(item, "ASCII", NULL);
    variable_item_add_value(item, "HEX", NULL);
    variable_item_add_value(item, "Both", NULL);
    variable_item_set_current_value_index(item, view->display_mode);
    
    item = variable_item_list_add(view->var_list, "Line Ending", 4, serial_cmd_var_line_changed, view);
    variable_item_add_value(item, "None", NULL);
    variable_item_add_value(item, "CR", NULL);
    variable_item_add_value(item, "LF", NULL);
    variable_item_add_value(item, "CRLF", NULL);
    variable_item_set_current_value_index(item, view->line_ending);
    
    item = variable_item_list_add(view->var_list, "Logging", 2, serial_cmd_var_logging_changed, view);
    variable_item_add_value(item, "Enabled", NULL);
    variable_item_add_value(item, "Disabled", NULL);
    variable_item_set_current_value_index(item, 1);
    
    item = variable_item_list_add(view->var_list, "Clear Buffer", 0, serial_cmd_clear_callback, view);
    variable_item_set_current_value_text(item, "Press OK");
    
    view->terminal_widget = widget_alloc();
    widget_add_button_element(view->terminal_widget, GuiButtonTypeRight, "Send", serial_cmd_send_button_callback, view);
    widget_add_button_element(view->terminal_widget, GuiButtonTypeLeft, "Back", serial_cmd_back_callback, view);
    
    view->status_widget = widget_alloc();
    widget_add_string_element(view->status_widget, 64, 20, AlignCenter, AlignTop, FontPrimary, "Serial Commander");
    widget_add_string_element(view->status_widget, 64, 40, AlignCenter, AlignTop, FontSecondary, "Connect to start");
    widget_add_string_element(view->status_widget, 64, 56, AlignCenter, AlignTop, FontSecondary, "TX: C4  RX: C5 (3.3V)");
    widget_add_button_element(view->status_widget, GuiButtonTypeRight, "Back", status_back_callback, view);
    
    view->text_input = text_input_alloc();
    text_input_set_header_text(view->text_input, "Send Data");
    text_input_set_result_callback(view->text_input, serial_cmd_send_callback, view, view->tx_buffer, sizeof(view->tx_buffer), false);
    
    view_dispatcher_add_view(view_dispatcher, 0, variable_item_list_get_view(view->var_list));
    view_dispatcher_add_view(view_dispatcher, 1, widget_get_view(view->terminal_widget));
    view_dispatcher_add_view(view_dispatcher, 2, widget_get_view(view->status_widget));
    view_dispatcher_switch_to_view(view_dispatcher, 2);
    
    view_dispatcher_set_navigation_event_callback(view_dispatcher, serial_cmd_nav_callback);
}