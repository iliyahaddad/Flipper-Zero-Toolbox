/**
 * Flipper Toolbox - Device Profiles Module
 */

#ifndef DEVICE_PROFILES_H
#define DEVICE_PROFILES_H

#include "shared.h"

typedef struct DeviceProfiles DeviceProfiles;

void device_profiles_start(ViewDispatcher* view_dispatcher, Settings* settings);

#endif // DEVICE_PROFILES_H