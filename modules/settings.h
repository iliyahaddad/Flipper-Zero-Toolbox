/**
 * Flipper Toolbox - Settings Module
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "shared.h"

typedef struct Settings Settings;

Settings* settings_alloc(void);
void settings_free(Settings* settings);

bool settings_load(Settings* settings);
bool settings_save(Settings* settings);

uint32_t settings_get_default_baud(Settings* settings);
void settings_set_default_baud(Settings* settings, uint32_t baud);

DisplayMode settings_get_default_display_mode(Settings* settings);
void settings_set_default_display_mode(Settings* settings, DisplayMode mode);

LineEnding settings_get_default_line_ending(Settings* settings);
void settings_set_default_line_ending(Settings* settings, LineEnding le);

bool settings_get_confirm_on_delete(Settings* settings);
void settings_set_confirm_on_delete(Settings* settings, bool confirm);

void settings_start(ViewDispatcher* view_dispatcher, Settings* settings);

#endif // SETTINGS_H