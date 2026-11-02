/**
 * Flipper Toolbox - GPIO Laboratory Module Implementation
 * 
 * Provides GPIO pin reading/writing and basic diagnostics.
 * WARNING: Always verify pinout and voltage levels before connecting external devices!
 * Flipper Zero GPIO operates at 3.3V logic level.
 */

#include "gpio_laboratory.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>

#define TAG "GpioLab"

static const char* gpio_mode_names[] = {"Input", "Output PP", "Output OD", "Analog"};

typedef struct {
    VariableItemList* var_list;
    ViewDispatcher* view_dispatcher;
    Widget* warning_widget;
    Widget* status_widget;
    NotificationApp* notifications;
    
    const GpioPinRecord* current_pin_record;
    uint8_t pin_index;
    uint8_t current_mode;
    bool output_state;
    bool pin_configured;
} GpioLabView;

static void gpio_lab_warning_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    GpioLabView* view = context;
    view_dispatcher_switch_to_view(view->view_dispatcher, 1);
}

static void gpio_lab_pin_changed(VariableItem* item) {
    GpioLabView* view = variable_item_get_context(item);
    view->pin_index = variable_item_get_current_value_index(item);
    view->current_pin_record = furi_hal_resources_pin_by_number(view->pin_index);
    
    if(view->pin_configured && view->current_pin_record) {
        furi_hal_gpio_init(view->current_pin_record->pin, GpioModeAnalog, GpioPullNo, GpioSpeedVeryHigh);
        view->pin_configured = false;
    }
}

static void gpio_lab_mode_changed(VariableItem* item) {
    GpioLabView* view = variable_item_get_context(item);
    view->current_mode = variable_item_get_current_value_index(item);
    
    if(view->pin_configured && view->current_pin_record) {
        furi_hal_gpio_init(view->current_pin_record->pin, GpioModeAnalog, GpioPullNo, GpioSpeedVeryHigh);
        view->pin_configured = false;
    }
}

static void gpio_lab_read_callback(VariableItem* item) {
    GpioLabView* view = variable_item_get_context(item);
    UNUSED(item);
    
    if(!view->current_pin_record) return;
    
    if(!view->pin_configured) {
        GpioMode mode = (view->current_mode == 0) ? GpioModeInput : GpioModeAnalog;
        furi_hal_gpio_init(view->current_pin_record->pin, mode, GpioPullNo, GpioSpeedVeryHigh);
        view->pin_configured = true;
    }
    
    bool state = furi_hal_gpio_read(view->current_pin_record->pin);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Pin: %s\nMode: %s\nState: %s",
             view->current_pin_record->name,
             gpio_mode_names[view->current_mode],
             state ? "HIGH (1)" : "LOW (0)");
    widget_reset(view->status_widget);
    widget_add_text_box_element(view->status_widget, 4, 4, 120, 56, AlignLeft, AlignTop, buffer, false);
    view_dispatcher_switch_to_view(view->view_dispatcher, 2);
}

static void gpio_lab_write_callback(VariableItem* item) {
    GpioLabView* view = variable_item_get_context(item);
    UNUSED(item);
    
    if(!view->current_pin_record) return;
    
    if(view->current_mode != 1 && view->current_mode != 2) {
        widget_reset(view->status_widget);
        widget_add_string_element(view->status_widget, 4, 4, AlignLeft, AlignTop, FontSecondary, "Error: Set mode to Output first");
        view_dispatcher_switch_to_view(view->view_dispatcher, 2);
        return;
    }
    
    if(!view->pin_configured) {
        GpioMode mode = (view->current_mode == 1) ? GpioModeOutputPushPull : GpioModeOutputOpenDrain;
        furi_hal_gpio_init(view->current_pin_record->pin, mode, GpioPullNo, GpioSpeedVeryHigh);
        view->pin_configured = true;
    }
    
    view->output_state = !view->output_state;
    furi_hal_gpio_write(view->current_pin_record->pin, view->output_state);
    
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Pin: %s\nMode: %s\nOutput: %s",
             view->current_pin_record->name,
             gpio_mode_names[view->current_mode],
             view->output_state ? "HIGH (1)" : "LOW (0)");
    widget_reset(view->status_widget);
    widget_add_text_box_element(view->status_widget, 4, 4, 120, 56, AlignLeft, AlignTop, buffer, false);
    view_dispatcher_switch_to_view(view->view_dispatcher, 2);
    
    notification_message(view->notifications, &sequence_single_vibro);
}

static void gpio_lab_toggle_callback(VariableItem* item) {
    gpio_lab_write_callback(item);
}

static void gpio_lab_test_pattern_callback(VariableItem* item) {
    GpioLabView* view = variable_item_get_context(item);
    UNUSED(item);
    
    if(!view->current_pin_record) return;
    
    if(view->current_mode != 1 && view->current_mode != 2) {
        widget_reset(view->status_widget);
        widget_add_string_element(view->status_widget, 4, 4, AlignLeft, AlignTop, FontSecondary, "Error: Set mode to Output first");
        view_dispatcher_switch_to_view(view->view_dispatcher, 2);
        return;
    }
    
    if(!view->pin_configured) {
        GpioMode mode = (view->current_mode == 1) ? GpioModeOutputPushPull : GpioModeOutputOpenDrain;
        furi_hal_gpio_init(view->current_pin_record->pin, mode, GpioPullNo, GpioSpeedVeryHigh);
        view->pin_configured = true;
    }
    
    for(int i = 0; i < 10; i++) {
        furi_hal_gpio_write(view->current_pin_record->pin, true);
        furi_delay_ms(100);
        furi_hal_gpio_write(view->current_pin_record->pin, false);
        furi_delay_ms(100);
    }
    
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Pin: %s\nTest pattern complete\n10 pulses generated",
             view->current_pin_record->name);
    widget_reset(view->status_widget);
    widget_add_text_box_element(view->status_widget, 4, 4, 120, 56, AlignLeft, AlignTop, buffer, false);
    view_dispatcher_switch_to_view(view->view_dispatcher, 2);
}

static void gpio_lab_back_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    GpioLabView* view = context;
    if(view->pin_configured && view->current_pin_record) {
        furi_hal_gpio_init(view->current_pin_record->pin, GpioModeAnalog, GpioPullNo, GpioSpeedVeryHigh);
        view->pin_configured = false;
    }
    view_dispatcher_stop(view->view_dispatcher);
    variable_item_list_free(view->var_list);
    widget_free(view->warning_widget);
    widget_free(view->status_widget);
    free(view);
}

static bool gpio_lab_nav_callback(void* context) {
    GpioLabView* view = context;
    if(view->pin_configured && view->current_pin_record) {
        furi_hal_gpio_init(view->current_pin_record->pin, GpioModeAnalog, GpioPullNo, GpioSpeedVeryHigh);
        view->pin_configured = false;
    }
    view_dispatcher_stop(view->view_dispatcher);
    variable_item_list_free(view->var_list);
    widget_free(view->warning_widget);
    widget_free(view->status_widget);
    free(view);
    return true;
}

static void status_back_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    GpioLabView* view = context;
    view_dispatcher_switch_to_view(view->view_dispatcher, 1);
}

void gpio_laboratory_start(ViewDispatcher* view_dispatcher, NotificationApp* notifications) {
    GpioLabView* view = malloc(sizeof(GpioLabView));
    view->view_dispatcher = view_dispatcher;
    view->notifications = notifications;
    view->pin_index = 0;
    view->current_pin_record = furi_hal_resources_pin_by_number(0);
    view->current_mode = 0;
    view->output_state = false;
    view->pin_configured = false;
    
    view->warning_widget = widget_alloc();
    widget_add_string_element(view->warning_widget, 64, 10, AlignCenter, AlignTop, FontPrimary, "WARNING");
    widget_add_string_element(view->warning_widget, 64, 28, AlignCenter, AlignTop, FontSecondary, "GPIO is 3.3V logic");
    widget_add_string_element(view->warning_widget, 64, 40, AlignCenter, AlignTop, FontSecondary, "Verify pinout before");
    widget_add_string_element(view->warning_widget, 64, 52, AlignCenter, AlignTop, FontSecondary, "connecting devices!");
    widget_add_button_element(view->warning_widget, GuiButtonTypeRight, "OK", gpio_lab_warning_callback, view);
    widget_add_button_element(view->warning_widget, GuiButtonTypeLeft, "Back", gpio_lab_back_callback, view);
    
    view->var_list = variable_item_list_alloc();
    
    VariableItem* item = variable_item_list_add(view->var_list, "Pin", 0, gpio_lab_pin_changed, view);
    for(int i = 0; i < 32; i++) {
        const GpioPinRecord* pin_rec = furi_hal_resources_pin_by_number(i);
        if(pin_rec) {
            variable_item_set_values_count(item, i + 1);
            variable_item_set_current_value_text(item, pin_rec->name);
        }
    }
    
    item = variable_item_list_add(view->var_list, "Mode", 4, gpio_lab_mode_changed, view);
    for(int i = 0; i < 4; i++) {
        variable_item_add_value(item, gpio_mode_names[i], NULL);
    }
    
    item = variable_item_list_add(view->var_list, "Read Input", 0, gpio_lab_read_callback, view);
    variable_item_set_current_value_text(item, "Press OK");
    
    item = variable_item_list_add(view->var_list, "Write Output", 0, gpio_lab_write_callback, view);
    variable_item_set_current_value_text(item, "Press OK");
    
    item = variable_item_list_add(view->var_list, "Toggle Output", 0, gpio_lab_toggle_callback, view);
    variable_item_set_current_value_text(item, "Press OK");
    
    item = variable_item_list_add(view->var_list, "Test Pattern", 0, gpio_lab_test_pattern_callback, view);
    variable_item_set_current_value_text(item, "Press OK");
    
    view->status_widget = widget_alloc();
    widget_add_button_element(view->status_widget, GuiButtonTypeRight, "Back", status_back_callback, view);
    
    view_dispatcher_add_view(view_dispatcher, 0, widget_get_view(view->warning_widget));
    view_dispatcher_add_view(view_dispatcher, 1, variable_item_list_get_view(view->var_list));
    view_dispatcher_add_view(view_dispatcher, 2, widget_get_view(view->status_widget));
    view_dispatcher_switch_to_view(view_dispatcher, 0);
    
    view_dispatcher_set_navigation_event_callback(view_dispatcher, gpio_lab_nav_callback);
}