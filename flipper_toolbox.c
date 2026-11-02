/**
 * Flipper Toolbox - Modular Electronics, Serial Communication, and Network Toolkit
 * 
 * Main application entry point and module management.
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "flipper_toolbox_icons.h"
#include "modules/serial_commander.h"
#include "modules/gpio_laboratory.h"
#include "modules/network_calculator.h"
#include "modules/electronics_calculator.h"
#include "modules/device_profiles.h"
#include "modules/session_logs.h"
#include "modules/protocol_learning.h"
#include "modules/settings.h"

#define TAG "FlipperToolbox"

#define VIEW_MAIN_MENU 0
#define VIEW_ABOUT 1

typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* main_menu;
    Widget* about_widget;
    NotificationApp* notifications;
    Settings* settings;
} FlipperToolbox;

static void main_menu_callback(void* context, uint32_t index) {
    FlipperToolbox* app = context;
    switch(index) {
        case 0: // Serial Commander
            serial_commander_start(app->view_dispatcher, app->notifications, app->settings);
            break;
        case 1: // GPIO Laboratory
            gpio_laboratory_start(app->view_dispatcher, app->notifications);
            break;
        case 2: // Network Calculator
            network_calculator_start(app->view_dispatcher);
            break;
        case 3: // Electronics Calculator
            electronics_calculator_start(app->view_dispatcher);
            break;
        case 4: // Device Profiles
            device_profiles_start(app->view_dispatcher, app->settings);
            break;
        case 5: // Session Logs
            session_logs_start(app->view_dispatcher);
            break;
        case 6: // Protocol Learning
            protocol_learning_start(app->view_dispatcher);
            break;
        case 7: // Settings
            settings_start(app->view_dispatcher, app->settings);
            break;
        case 8: // About
            view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_ABOUT);
            break;
    }
}

static void about_exit_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    FlipperToolbox* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_MAIN_MENU);
}

static FlipperToolbox* flipper_toolbox_alloc() {
    FlipperToolbox* app = malloc(sizeof(FlipperToolbox));
    
    app->view_dispatcher = view_dispatcher_alloc();
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->settings = settings_alloc();
    
    // Main Menu
    app->main_menu = submenu_alloc();
    submenu_set_header(app->main_menu, "FLIPPER TOOLBOX");
    submenu_add_item(app->main_menu, "Serial Commander", 0, main_menu_callback, app);
    submenu_add_item(app->main_menu, "GPIO Laboratory", 1, main_menu_callback, app);
    submenu_add_item(app->main_menu, "Network Calculator", 2, main_menu_callback, app);
    submenu_add_item(app->main_menu, "Electronics Calculator", 3, main_menu_callback, app);
    submenu_add_item(app->main_menu, "Device Profiles", 4, main_menu_callback, app);
    submenu_add_item(app->main_menu, "Session Logs", 5, main_menu_callback, app);
    submenu_add_item(app->main_menu, "Protocol Learning", 6, main_menu_callback, app);
    submenu_add_item(app->main_menu, "Settings", 7, main_menu_callback, app);
    submenu_add_item(app->main_menu, "About", 8, main_menu_callback, app);
    view_dispatcher_add_view(app->view_dispatcher, VIEW_MAIN_MENU, submenu_get_view(app->main_menu));
    
    // About Widget
    app->about_widget = widget_alloc();
    widget_add_string_element(app->about_widget, 64, 4, AlignCenter, AlignTop, FontPrimary, "Flipper Toolbox");
    widget_add_string_element(app->about_widget, 64, 20, AlignCenter, AlignTop, FontSecondary, "v0.2.0");
    widget_add_string_element(app->about_widget, 64, 36, AlignCenter, AlignTop, FontSecondary, "Electronics & Network Toolkit");
    widget_add_string_element(app->about_widget, 64, 52, AlignCenter, AlignTop, FontSecondary, "Firmware: 1.5.1+");
    widget_add_button_element(app->about_widget, GuiButtonTypeRight, "Back", about_exit_callback, app);
    view_dispatcher_add_view(app->view_dispatcher, VIEW_ABOUT, widget_get_view(app->about_widget));
    
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_MAIN_MENU);
    
    return app;
}

static void flipper_toolbox_free(FlipperToolbox* app) {
    settings_free(app->settings);
    furi_record_close(RECORD_NOTIFICATION);
    view_dispatcher_remove_view(app->view_dispatcher, VIEW_ABOUT);
    view_dispatcher_remove_view(app->view_dispatcher, VIEW_MAIN_MENU);
    widget_free(app->about_widget);
    submenu_free(app->main_menu);
    view_dispatcher_free(app->view_dispatcher);
    free(app);
}

int32_t flipper_toolbox_app(void* p) {
    UNUSED(p);
    FlipperToolbox* app = flipper_toolbox_alloc();
    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_run(app->view_dispatcher);
    furi_record_close(RECORD_GUI);
    flipper_toolbox_free(app);
    return 0;
}