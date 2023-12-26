#include "internal.h"
#include "index/index.h"
#include "api/api.h"

void controller_callback(struct evhttp_request *request, void *data) {
    (void)data;
    
    const char *uri_str = evhttp_request_get_uri(request);
    ASSERT(uri_str, HTTP_INTERNAL);

    LOG(LOG_DEBUG, "Reciving request to: %s\n", uri_str);

    if (STR_START_WITH(uri_str, "/index/")) {
        index_controller(request);
        goto done;
    }
    if (STR_START_WITH(uri_str, "/api/")) {
        api_controller(request);
        goto done;
    }

    LOG(LOG_DEBUG, "%s : Sending NOTFOUND!\n", __func__);
    evhttp_send_error(request, HTTP_NOTFOUND, NULL);

done:
    fflush(stdout);  // for docker log
}