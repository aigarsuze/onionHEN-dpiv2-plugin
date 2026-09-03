#pragma once

#include <stdint.h>

typedef struct plugin_settings {
    int enabled;
    uint16_t api_port;
    uint16_t webui_port;
} plugin_settings;

void plugin_settings_defaults(plugin_settings *settings);
int plugin_settings_load(const char *path, plugin_settings *settings);
int plugin_settings_save(const char *path, const plugin_settings *settings);
