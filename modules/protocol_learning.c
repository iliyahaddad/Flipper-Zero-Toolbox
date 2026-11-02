/**
 * Flipper Toolbox - Protocol Learning Module Implementation
 * 
 * Educational module for learning communication protocols.
 * Uses existing firmware APIs for IR, NFC, RFID, Sub-GHz file inspection.
 */

#include "protocol_learning.h"
#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <storage/storage.h>

#define TAG "ProtoLearn"

typedef struct {
    VariableItemList* var_list;
    ViewDispatcher* view_dispatcher;
    Widget* content_widget;
    
    int current_topic;
} ProtocolLearnView;

static const char* topics[] = {
    "IR Protocols",
    "NFC/RFID Basics",
    "Sub-GHz Protocols",
    "UART Framing",
    "Digital Signals",
    "HEX & Binary",
    "Sample Files",
};

static const char* topic_content[] = {
    "IR Protocols:\n"
    "- NEC: 32-bit, widely used\n"
    "- RC5/RC6: Philips protocols\n"
    "- Samsung: 32-bit variant\n"
    "- Sony: 12/15/20 bit\n"
    "- Raw: Timing arrays\n\n"
    "Flipper stores .ir files with:\n"
    "- Protocol name\n"
    "- Frequency (kHz)\n"
    "- Duty cycle\n"
    "- Timing data\n\n"
    "Use IR app to capture/analyze",
    
    "NFC/RFID Basics:\n"
    "- 13.56 MHz (NFC)\n"
    "- 125 kHz (LF RFID)\n\n"
    "NFC Types:\n"
    "- Type 1-5 (ISO 14443/15693)\n"
    "- MIFARE Classic/Ultralight\n"
    "- FeliCa, NTAG\n\n"
    "LF RFID:\n"
    "- EM4100, HID Prox\n"
    "- FDX-B (animal tags)\n\n"
    "Files: .nfc, .rfid",
    
    "Sub-GHz Protocols:\n"
    "- 300-928 MHz bands\n"
    "- AM/ASK/OOK modulation\n"
    "- FSK/GFSK modulation\n\n"
    "Common protocols:\n"
    "- Princeton (PT2262)\n"
    "- EV1527, HT6P20B\n"
    "- KeeLoq (rolling code)\n"
    "- Custom: raw data\n\n"
    "Files: .sub",
    
    "UART Framing:\n"
    "- Start bit (0)\n"
    "- 5-9 data bits\n"
    "- Parity: None/Even/Odd\n"
    "- Stop bits: 1/1.5/2\n\n"
    "Common formats:\n"
    "- 8N1: 8 data, no parity, 1 stop\n"
    "- 8E1: 8 data, even parity, 1 stop\n"
    "- Baud rates: 9600-115200\n\n"
    "Use Serial Commander to test",
    
    "Digital Signals:\n"
    "- HIGH/LOW (1/0)\n"
    "- TTL: 0-3.3V or 0-5V\n"
    "- CMOS: Rail-to-rail\n\n"
    "Timing:\n"
    "- Rise/fall time\n"
    "- Propagation delay\n"
    "- Setup/hold time\n\n"
    "Measure with GPIO Lab\nor oscilloscope",
    
    "HEX & Binary:\n"
    "Binary: Base-2 (0/1)\n"
    "Hex: Base-16 (0-9,A-F)\n\n"
    "Conversion:\n"
    "- 1 hex = 4 bits\n"
    "- 0xFF = 11111111 = 255\n\n"
    "Common prefixes:\n"
    "- 0b = binary\n"
    "- 0x = hex\n\n"
    "Use calculators to convert",
    
    "Sample Files:\n"
    "Stored in:\n"
    "- /ext/infrared/\n"
    "- /ext/nfc/\n"
    "- /ext/rfid/\n"
    "- /ext/subghz/\n\n"
    "File formats are documented\nin Flipper firmware repo.\n\n"
    "Use Protocol Learning to\ninspect captured signals.",
};

static void proto_topic_changed(VariableItem* item) {
    ProtocolLearnView* view = variable_item_get_context(item);
    view->current_topic = variable_item_get_current_value_index(item);
    
    widget_reset(view->content_widget);
    widget_add_text_box_element(view->content_widget, 4, 4, 120, 56, AlignLeft, AlignTop, topic_content[view->current_topic], false);
    view_dispatcher_switch_to_view(view->view_dispatcher, 1);
}

static bool proto_back_callback(void* context) {
    ProtocolLearnView* view = context;
    view_dispatcher_stop(view->view_dispatcher);
    variable_item_list_free(view->var_list);
    widget_free(view->content_widget);
    free(view);
    return true;
}

static void content_back_callback(GuiButtonType type, InputType event, void* context) {
    UNUSED(type);
    UNUSED(event);
    ProtocolLearnView* view = context;
    view_dispatcher_switch_to_view(view->view_dispatcher, 0);
}

void protocol_learning_start(ViewDispatcher* view_dispatcher) {
    ProtocolLearnView* view = malloc(sizeof(ProtocolLearnView));
    view->view_dispatcher = view_dispatcher;
    view->current_topic = 0;
    
    view->var_list = variable_item_list_alloc();
    
    VariableItem* item = variable_item_list_add(view->var_list, "Topics", 7, proto_topic_changed, view);
    for(int i = 0; i < 7; i++) {
        variable_item_add_value(item, topics[i], NULL);
    }
    
    view->content_widget = widget_alloc();
    widget_add_button_element(view->content_widget, GuiButtonTypeLeft, "Back", content_back_callback, view);
    
    view_dispatcher_add_view(view_dispatcher, 0, variable_item_list_get_view(view->var_list));
    view_dispatcher_add_view(view_dispatcher, 1, widget_get_view(view->content_widget));
    view_dispatcher_switch_to_view(view_dispatcher, 0);
    
    view_dispatcher_set_navigation_event_callback(view_dispatcher, proto_back_callback);
}