/**
 * Flipper Toolbox - Network Calculator Module
 */

#ifndef NETWORK_CALCULATOR_H
#define NETWORK_CALCULATOR_H

#include "shared.h"

typedef struct NetworkCalculator NetworkCalculator;

typedef struct {
    uint32_t ip_address;
    uint8_t cidr;
    uint32_t network_address;
    uint32_t broadcast_address;
    uint32_t first_usable;
    uint32_t last_usable;
    uint32_t total_addresses;
    uint32_t usable_hosts;
    uint32_t wildcard_mask;
    bool is_private;
    bool is_special;
    char special_type[32];
} NetworkCalcResult;

NetworkCalcResult network_calculate(uint32_t ip, uint8_t cidr);
bool network_parse_cidr(const char* input, uint32_t* ip, uint8_t* cidr);
bool network_validate_ip(uint32_t ip);
bool network_validate_cidr(uint8_t cidr);
const char* network_get_class(uint32_t ip);

void network_calculator_start(ViewDispatcher* view_dispatcher);

#endif // NETWORK_CALCULATOR_H