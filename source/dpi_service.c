#include "dpi_service.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "pkgserver_adapter.h"

#define LISTENER_READY_WAIT_MS 3000

static void *dpi_thread_main(void *argument) {
    dpi_service *service = (dpi_service *)argument;
    const int result = pkg_server_main();

    pthread_mutex_lock(&service->mutex);
    service->running = 0;
    pthread_mutex_unlock(&service->mutex);

    if (result != 0) {
        fprintf(stderr, "DPI v2 stopped with error %d\n", result);
    }
    return NULL;
}

static int wait_for_listener(dpi_service *service) {
    for (int waited = 0; waited < LISTENER_READY_WAIT_MS; waited += 50) {
        if (pkg_server_is_listening()) return 1;

        pthread_mutex_lock(&service->mutex);
        const int active = service->running;
        pthread_mutex_unlock(&service->mutex);
        if (!active) return 0;
        usleep(50 * 1000);
    }
    return pkg_server_is_listening() != 0;
}

int dpi_service_init(dpi_service *service, uint16_t api_port,
                     uint16_t webui_port) {
    if (!service || api_port == 0 || webui_port == 0 ||
        api_port == webui_port) {
        return 0;
    }
    memset(service, 0, sizeof(*service));
    if (pthread_mutex_init(&service->mutex, NULL) != 0) return 0;
    service->api_port = api_port;
    service->webui_port = webui_port;
    service->initialized = 1;
    return 1;
}

void dpi_service_stop(dpi_service *service) {
    if (!service || !service->initialized) return;

    pthread_t thread = {0};
    int join_thread = 0;
    pthread_mutex_lock(&service->mutex);
    if (service->thread_created) {
        pkg_server_request_stop();
        thread = service->thread;
        join_thread = 1;
    }
    pthread_mutex_unlock(&service->mutex);

    if (join_thread) pthread_join(thread, NULL);

    pthread_mutex_lock(&service->mutex);
    service->running = 0;
    service->thread_created = 0;
    pthread_mutex_unlock(&service->mutex);
}

void dpi_service_destroy(dpi_service *service) {
    if (!service || !service->initialized) return;
    dpi_service_stop(service);
    pthread_mutex_destroy(&service->mutex);
    memset(service, 0, sizeof(*service));
}

int dpi_service_start(dpi_service *service) {
    if (!service || !service->initialized) return 0;
    dpi_service_stop(service);

    pthread_mutex_lock(&service->mutex);
    service->running = 1;
    pkg_server_prepare();
    pkg_server_set_ports(service->api_port, service->webui_port);
    const int result = pthread_create(
        &service->thread, NULL, dpi_thread_main, service);
    if (result == 0) {
        service->thread_created = 1;
    } else {
        service->running = 0;
    }
    pthread_mutex_unlock(&service->mutex);

    if (result != 0 || !wait_for_listener(service)) {
        dpi_service_stop(service);
        return 0;
    }
    return 1;
}

int dpi_service_restart(dpi_service *service) {
    return dpi_service_start(service);
}

int dpi_service_reconfigure(dpi_service *service, uint16_t api_port,
                            uint16_t webui_port) {
    if (!service || !service->initialized || api_port == 0 ||
        webui_port == 0 || api_port == webui_port) {
        return 0;
    }

    pthread_mutex_lock(&service->mutex);
    const uint16_t previous_api_port = service->api_port;
    const uint16_t previous_webui_port = service->webui_port;
    const int was_running = service->running;
    if (!was_running) {
        service->api_port = api_port;
        service->webui_port = webui_port;
    }
    pthread_mutex_unlock(&service->mutex);
    if (!was_running) return 1;

    dpi_service_stop(service);
    pthread_mutex_lock(&service->mutex);
    service->api_port = api_port;
    service->webui_port = webui_port;
    pthread_mutex_unlock(&service->mutex);
    if (dpi_service_start(service)) return 1;

    pthread_mutex_lock(&service->mutex);
    service->api_port = previous_api_port;
    service->webui_port = previous_webui_port;
    pthread_mutex_unlock(&service->mutex);
    (void)dpi_service_start(service);
    return 0;
}

int dpi_service_running(dpi_service *service) {
    if (!service || !service->initialized) return 0;
    pthread_mutex_lock(&service->mutex);
    const int active = service->running;
    pthread_mutex_unlock(&service->mutex);
    return active && pkg_server_is_listening();
}
