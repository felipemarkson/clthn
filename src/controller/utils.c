#include "utils.h"

int parse_query(struct evhttp_request *request, struct evkeyvalq empty_queu[1]) {
    const char *uri_str = evhttp_request_get_uri(request);
    if (uri_str == NULL)
        return -2;  // Internal ERROR;

    struct evhttp_uri *uri = evhttp_uri_parse(uri_str);
    if (uri == NULL) {
        return -2;  // Internal ERROR;
    }

    const char *query = evhttp_uri_get_query(uri);
    if (query == NULL) {
        evhttp_uri_free(uri);
        return -1;
    }

    TAILQ_INIT(empty_queu);
    if (evhttp_parse_query_str(query, empty_queu) != 0) {
        evhttp_uri_free(uri);
        evhttp_clear_headers(empty_queu);
        *empty_queu = (struct evkeyvalq){0};
        return -1;
    }

    evhttp_uri_free(uri);

    return 0;
}
