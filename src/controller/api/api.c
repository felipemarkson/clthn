#include "api.h"

void api_controller(struct evhttp_request *request) {
    evhttp_send_reply(request, HTTP_OK, "OK", NULL);
}