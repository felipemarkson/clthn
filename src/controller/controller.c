#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/http.h>
#include <event2/keyvalq_struct.h>
#include <string.h>
#include <sys/queue.h>

#define STR_START_WITH(cstr, token) (strncmp((cstr), (token), strlen(token)) == 0)
#define STR_EQ(cstr, token) (strcmp((cstr), (token)) == 0)
#define METHOD(request) evhttp_request_get_command(request)

#define ASSERT(check, code_on_error)                                                     \
    do {                                                                                 \
        if (!(check)) {                                                                  \
            evhttp_send_error(request, (code_on_error), NULL);                           \
            fprintf(stderr, "ERROR on %s:%d Caused by: %s\n", __FILE__, __LINE__, #check); \
            fflush(stderr);                                                              \
            goto done;                                                                   \
        }                                                                                \
    } while (0)

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

void parse(struct evhttp_request *request) {
    struct evkeyvalq kv_queu = {0};
    struct evbuffer *evb = NULL;
    struct evkeyvalq *headers = NULL;

    ASSERT(parse_query(request, &kv_queu) == 0, HTTP_BADREQUEST);

    ASSERT(evb = evbuffer_new(), HTTP_INTERNAL);

    ASSERT(evbuffer_add_printf(evb, "<ul>") > 0, HTTP_INTERNAL);
    for (struct evkeyval *kv = kv_queu.tqh_first; kv; kv = kv->next.tqe_next) {
        int nbytes = evbuffer_add_printf(evb, "<li>%s: %s</li>", kv->key, kv->value);
        ASSERT(nbytes > 0, HTTP_INTERNAL);
    }
    ASSERT(evbuffer_add_printf(evb, "</ul>\n") > 0, HTTP_INTERNAL);

    ASSERT(headers = evhttp_request_get_output_headers(request), HTTP_INTERNAL);
    ASSERT(evhttp_add_header(headers, "Content-Type", "text/html") == 0, HTTP_INTERNAL);
    evhttp_send_reply(request, HTTP_OK, "OK", evb);

done:
    if(evb) evbuffer_free(evb);
}

void answer(struct evhttp_request *request) {
    struct evbuffer *evb = NULL;
    struct evkeyvalq *headers = NULL;

    ASSERT(evb = evbuffer_new(), HTTP_INTERNAL);

    int nbytes = evbuffer_add_printf(evb,
                                     "<button class=\"w-full h-full\" hx-get=\"/index/view_question\" hx-swap=\"outerHTML\">"
                                     "The server says %d.<br/>Click on me again!"
                                     "</button>\n",
                                     42);
    ASSERT(nbytes > 0, HTTP_INTERNAL);

    ASSERT(headers = evhttp_request_get_output_headers(request), HTTP_INTERNAL);
    ASSERT(evhttp_add_header(headers, "Content-Type", "text/html") == 0, HTTP_INTERNAL);
    evhttp_send_reply(request, HTTP_OK, "OK", evb);

done:
    if(evb) evbuffer_free(evb);
}

void question(struct evhttp_request *request) {
    struct evbuffer *evb = NULL;
    struct evkeyvalq *headers = NULL;

    ASSERT(evb = evbuffer_new(), HTTP_INTERNAL);
    int nbytes = evbuffer_add_printf(evb,
                                     "<button class=\"w-full h-full\" hx-get=\"/index/view_answer\" hx-swap=\"outerHTML\">"
                                     "I was rendered on server.<br/>Click on me to get the answer."
                                     "</button>\n");
    ASSERT(nbytes > 0, HTTP_INTERNAL);
    ASSERT(headers = evhttp_request_get_output_headers(request), HTTP_INTERNAL);
    ASSERT(evhttp_add_header(headers, "Content-Type", "text/html") == 0, HTTP_INTERNAL);
    evhttp_send_reply(request, HTTP_OK, "OK", evb);

done:
    if(evb) evbuffer_free(evb);
}

void index_controller(struct evhttp_request *request) {
    const char *uri_str = evhttp_request_get_uri(request);
    ASSERT(uri_str, HTTP_INTERNAL);

    if ((STR_START_WITH(uri_str, "/index/view_parse") && METHOD(request) == EVHTTP_REQ_GET)) {
        parse(request);
        return;
    }

    if ((STR_EQ(uri_str, "/index/view_answer") && METHOD(request) == EVHTTP_REQ_GET)) {
        answer(request);
        return;
    }

    if ((STR_EQ(uri_str, "/index/view_question") && METHOD(request) == EVHTTP_REQ_GET)) {
        question(request);
        return;
    }

done:
    evhttp_send_error(request, HTTP_NOTFOUND, NULL);
}

void controller_callback(struct evhttp_request *request, void *data) {
    (void)data;
    const char *uri_str = evhttp_request_get_uri(request);
    ASSERT(uri_str, HTTP_INTERNAL);

    printf("Reciving from: %s\n", uri_str);

    if (STR_START_WITH(uri_str, "/index/")) {
        index_controller(request);
        goto done;
    }

    evhttp_send_error(request, HTTP_NOTFOUND, NULL);

done:
    fflush(stdout);  // for docker log
}