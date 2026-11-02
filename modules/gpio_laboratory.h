/**
 * Flipper Toolbox - GPIO Laboratory Module
 */

#ifndef GPIO_LABORATORY_H
#define GPIO_LABORATORY_H

#include "shared.h"

typedef struct GpioLaboratory GpioLaboratory;

void gpio_laboratory_start(ViewDispatcher* view_dispatcher, NotificationApp* notifications);

#endif // GPIO_LABORATORY_H