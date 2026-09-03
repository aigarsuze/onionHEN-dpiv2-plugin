#pragma once

#include <pthread.h>
#include <stdint.h>

typedef struct dpi_service {
    pthread_mutex_t mutex;
    pthread_t thread;
    uint16_t api_port;
    uint16_t webui_port;
    int initialized;
    int running;
    int thread_created;
} dpi_service;

int dpi_service_init(dpi_service *service, uint16_t api_port,
                     uint16_t webui_port);
void dpi_service_destroy(dpi_service *service);
int dpi_service_start(dpi_service *service);
void dpi_service_stop(dpi_service *service);
int dpi_service_restart(dpi_service *service);
int dpi_service_reconfigure(dpi_service *service, uint16_t api_port,
                            uint16_t webui_port);
int dpi_service_running(dpi_service *service);
