#include "controller/controller.h"
#include "httpserver/httpserver.h"

#define MAX_BODY_SIZE 512
#define MAX_HEAD_SIZE 512
#define TIMEOUT 10
#define PORT 5555

int main(void) {
    printf("Initializing the server!\n");
    HttpServer httpServer = {0};
    if (initHTTPServer(&httpServer) != 0) {
        (void)fprintf(stderr, "[%s] %sn", "err", "initHTTPServer: Error on init HTTP Server");
    }

    evhttp_set_max_headers_size(httpServer.http, MAX_HEAD_SIZE);
    evhttp_set_max_body_size(httpServer.http, MAX_BODY_SIZE);
    evhttp_set_timeout(httpServer.http, TIMEOUT);
    evhttp_set_allowed_methods(httpServer.http, EVHTTP_REQ_GET | EVHTTP_REQ_POST);

    evhttp_set_gencb(httpServer.http, controller_callback, NULL);

    if (bindHTTPServer(&httpServer, "0.0.0.0", PORT) != 0) {
        (void)fprintf(stderr, "[%s] %sn", "err", "bindHTTPServer: Error on bind HTTP Server");
    }

    event_base_dispatch(httpServer.base);
    freeHTTPServer(&httpServer);

    return 0;
}