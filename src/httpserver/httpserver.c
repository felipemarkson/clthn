#include "httpserver.h"
#include <signal.h>

static void shuntdown(int sig, short events, void *arg) {
    (void)events;
    (void)sig;
    struct event_base *base = arg;
    event_base_loopbreak(base);
    printf("Gracefull exit! :)\n");
    fflush(stdout);
}

void freeHTTPServer(HttpServer httpServer[1]) {
    event_free(httpServer->term);
    evhttp_free(httpServer->http);
    event_base_free(httpServer->base);
    event_config_free(httpServer->cfg);
    *httpServer = (HttpServer){0};
}

int initHTTPServer(HttpServer httpServer[1]) {
    struct event_base *base = NULL;
    struct event_config *cfg = NULL;
    {  // base creation
        cfg = event_config_new();
        if (!cfg) {
            return -1;
        }
        base = event_base_new_with_config(cfg);
        if (!base) {
            event_config_free(cfg);
            return -1;
        }
    }

    struct evhttp *http = NULL;
    {  // http creation
        http = evhttp_new(base);
        if (!http) {
            event_base_free(base);
            event_config_free(cfg);
            return -1;
        }
    }

    struct event *term = evsignal_new(base, SIGINT | SIGTERM , shuntdown, base);
    if (!term) {
        evhttp_free(http);
        event_base_free(base);
        event_config_free(cfg);
        return -1;
    }
    if (event_add(term, NULL) != 0) {
        event_free(term);
        evhttp_free(http);
        event_base_free(base);
        event_config_free(cfg);
        return -1;
    }

    httpServer->base = base;
    httpServer->http = http;
    httpServer->cfg = cfg;
    httpServer->handle = NULL;
    httpServer->term = term;
    return 0;
}

int bindHTTPServer(HttpServer httpServer[1], const char addr[], uint16_t port) {
    struct evhttp_bound_socket *handle = evhttp_bind_socket_with_handle(httpServer->http, addr, port);
    if (!handle) {
        return -1;
    }
    httpServer->handle = handle;
    return 0;
}