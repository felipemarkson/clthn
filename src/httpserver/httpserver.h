#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <event2/event.h>
#include <event2/http.h>

typedef struct {
    struct event_config *cfg;
    struct event_base *base;
    struct evhttp *http;
    struct evhttp_bound_socket *handle;
    struct event *term;
} HttpServer;

void freeHTTPServer(HttpServer httpServer[1]);
int initHTTPServer(HttpServer httpServer[1]);
int bindHTTPServer(HttpServer httpServer[1], const char addr[], uint16_t port);
#endif