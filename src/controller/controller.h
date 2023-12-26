#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <event2/http.h>

void controller_callback(struct evhttp_request *request, void *data);

#endif
