/**
 * Flipper Toolbox - Serial Commander Module
 */

#ifndef SERIAL_COMMANDER_H
#define SERIAL_COMMANDER_H

#include "shared.h"

typedef struct SerialCommander SerialCommander;

void serial_commander_start(ViewDispatcher* view_dispatcher, NotificationApp* notifications, Settings* settings);

#endif // SERIAL_COMMANDER_H