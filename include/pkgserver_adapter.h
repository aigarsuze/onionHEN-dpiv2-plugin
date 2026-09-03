#pragma once

#include <stdint.h>

#include "plugin_config.h"

#define ONION_PKGNET_PORT DPI_API_PORT
#define ONION_WEBUI_PORT DPI_WEBUI_PORT

int pkg_server_main(void);
void pkg_server_prepare(void);
void pkg_server_request_stop(void);
int pkg_server_is_listening(void);
void pkg_server_set_webui_lang(const char *lang);
void pkg_server_set_ports(uint16_t api_port, uint16_t webui_port);
