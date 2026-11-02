/**
 * Flipper Toolbox - Network Calculator Module Implementation
 */

#include "network_calculator.h"
#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>

#define TAG "NetworkCalc"

typedef struct {
    VariableItemList* var_list;
    ViewDispatcher* view_dispatcher;
    Widget* result_widget;
    NetworkCalcResult result;
    char ip_buffer[16];
    char cidr_buffer[4];
} NetworkCalcView;

static bool network_calc_nav_callback(void* context) {
    NetworkCalcView* view = context;
    view_dispatcher_stop(view->view_dispatcher);
    variable_item_list_free(view->var_list);
    widget_free(view->result_widget);
    free(view);
    return true;
}

static void network_calc_item_callback(VariableItem* item) {
    NetworkCalcView* view = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    (void)index;
    
    uint32_t ip = ip_str_to_uint(view->ip_buffer);
    uint8_t cidr = (uint8_t)atoi(view->cidr_buffer);
    
    if(!network_validate_ip(ip) || !network_validate_cidr(cidr)) {
        widget_reset(view->result_widget);
        widget_add_string_element(view->result_widget, 64, 32, AlignCenter, AlignTop, FontSecondary, "Invalid IP or CIDR");
        view_dispatcher_switch_to_view(view->view_dispatcher, 1);
        return;
    }
    
    view->result = network_calculate(ip, cidr);
    
    char buffer[512];
    char ip_str[16], net_str[16], bcast_str[16], first_str[16], last_str[16], wc_str[16];
    
    ip_uint_to_str(view->result.ip_address, ip_str, sizeof(ip_str));
    ip_uint_to_str(view->result.network_address, net_str, sizeof(net_str));
    ip_uint_to_str(view->result.broadcast_address, bcast_str, sizeof(bcast_str));
    ip_uint_to_str(view->result.first_usable, first_str, sizeof(first_str));
    ip_uint_to_str(view->result.last_usable, last_str, sizeof(last_str));
    ip_uint_to_str(view->result.wildcard_mask, wc_str, sizeof(wc_str));
    
    snprintf(buffer, sizeof(buffer),
        "IP: %s/%u\n"
        "Network: %s\n"
        "Broadcast: %s\n"
        "First: %s\n"
        "Last: %s\n"
        "Total: %lu\n"
        "Usable: %lu\n"
        "Wildcard: %s\n"
        "%s%s",
        ip_str, view->result.cidr,
        net_str, bcast_str,
        first_str, last_str,
        view->result.total_addresses,
        view->result.usable_hosts,
        wc_str,
        view->result.is_private ? "Private\n" : "Public\n",
        view->result.is_special ? view->result.special_type : "");
    
    widget_reset(view->result_widget);
    widget_add_text_box_element(view->result_widget, 4, 4, 120, 56, AlignLeft, AlignTop, buffer, false);
    view_dispatcher_switch_to_view(view->view_dispatcher, 1);
}

NetworkCalcResult network_calculate(uint32_t ip, uint8_t cidr) {
    NetworkCalcResult result = {0};
    result.ip_address = ip;
    result.cidr = cidr;
    
    uint32_t mask = cidr_to_mask(cidr);
    result.network_address = ip & mask;
    result.broadcast_address = ip | ~mask;
    result.wildcard_mask = ~mask;
    result.total_addresses = (cidr >= 31) ? (1u << (32 - cidr)) : (1u << (32 - cidr));
    
    if(cidr == 31) {
        result.first_usable = result.network_address;
        result.last_usable = result.broadcast_address;
        result.usable_hosts = 2;
    } else if(cidr == 32) {
        result.first_usable = result.network_address;
        result.last_usable = result.network_address;
        result.usable_hosts = 1;
    } else {
        result.first_usable = result.network_address + 1;
        result.last_usable = result.broadcast_address - 1;
        result.usable_hosts = result.total_addresses - 2;
    }
    
    result.is_private = is_private_ip(ip);
    result.is_special = false;
    result.special_type[0] = '\0';
    
    if(ip == 0) {
        result.is_special = true;
        strcpy(result.special_type, "Default route");
    } else if((ip & 0xFF000000) == 0x7F000000) {
        result.is_special = true;
        strcpy(result.special_type, "Loopback");
    } else if((ip & 0xFFFF0000) == 0xA9FE0000) {
        result.is_special = true;
        strcpy(result.special_type, "Link-local");
    } else if((ip & 0xF0000000) == 0xE0000000) {
        result.is_special = true;
        strcpy(result.special_type, "Multicast");
    } else if((ip & 0xF0000000) == 0xF0000000) {
        result.is_special = true;
        strcpy(result.special_type, "Reserved");
    } else if(ip == 0xFFFFFFFF) {
        result.is_special = true;
        strcpy(result.special_type, "Limited broadcast");
    }
    
    return result;
}

bool network_parse_cidr(const char* input, uint32_t* ip, uint8_t* cidr) {
    char buf[32];
    safe_strncpy(buf, input, sizeof(buf));
    char* slash = strchr(buf, '/');
    if(!slash) return false;
    *slash = '\0';
    *ip = ip_str_to_uint(buf);
    *cidr = (uint8_t)atoi(slash + 1);
    return network_validate_ip(*ip) && network_validate_cidr(*cidr);
}

bool network_validate_ip(uint32_t ip) {
    UNUSED(ip);
    return true;
}

bool network_validate_cidr(uint8_t cidr) {
    return cidr <= 32;
}

const char* network_get_class(uint32_t ip) {
    uint8_t first = (ip >> 24) & 0xFF;
    if(first <= 127) return "Class A";
    if(first <= 191) return "Class B";
    if(first <= 223) return "Class C";
    if(first <= 239) return "Class D (Multicast)";
    return "Class E (Reserved)";
}

void network_calculator_start(ViewDispatcher* view_dispatcher) {
    NetworkCalcView* view = malloc(sizeof(NetworkCalcView));
    view->view_dispatcher = view_dispatcher;
    strcpy(view->ip_buffer, "192.168.1.1");
    strcpy(view->cidr_buffer, "24");
    
    view->var_list = variable_item_list_alloc();
    
    VariableItem* item = variable_item_list_add(view->var_list, "IP Address", 0, network_calc_item_callback, view);
    variable_item_set_current_value_text(item, view->ip_buffer);
    
    item = variable_item_list_add(view->var_list, "CIDR", 0, network_calc_item_callback, view);
    variable_item_set_current_value_text(item, view->cidr_buffer);
    
    view->result_widget = widget_alloc();
    
    view_dispatcher_add_view(view_dispatcher, 0, variable_item_list_get_view(view->var_list));
    view_dispatcher_add_view(view_dispatcher, 1, widget_get_view(view->result_widget));
    view_dispatcher_switch_to_view(view_dispatcher, 0);
    
    view_dispatcher_set_navigation_event_callback(view_dispatcher, network_calc_nav_callback);
}