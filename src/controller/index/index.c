#include "index.h"

#include "../utils.h"

static void parse(struct evhttp_request *request) {
    struct evkeyvalq kv_queu = {0};
    struct evbuffer *evb = NULL;
    struct evkeyvalq *headers = NULL;

    int parser_ret = parse_query(request, &kv_queu);
    ASSERT(parser_ret = !-2, HTTP_INTERNAL);
    if (parser_ret < 0) {
        LOG(LOG_DEBUG, "Sending Invalid Query!");
        evhttp_send_error(request, HTTP_BADREQUEST, "Invalid Query");
        goto done;
    }

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
    if (evb) evbuffer_free(evb);
}

static void answer(struct evhttp_request *request) {
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
    if (evb) evbuffer_free(evb);
}

static void question(struct evhttp_request *request) {
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
    if (evb) evbuffer_free(evb);
}

void index_controller(struct evhttp_request *request) {
    const char *uri_str = evhttp_request_get_uri(request);
    ASSERT(uri_str, HTTP_INTERNAL);

    if (STR_START_WITH(uri_str, "/index/view_parse")) {
        if (METHOD(request) != EVHTTP_REQ_GET) {
            LOG(LOG_DEBUG, "%s : Sending BADMETHOD", __func__);
            evhttp_send_error(request, HTTP_BADMETHOD, NULL);
            goto done;
        }
        parse(request);
        return;
    }

    if (STR_EQ(uri_str, "/index/view_answer")) {
        if (METHOD(request) != EVHTTP_REQ_GET) {
            LOG(LOG_DEBUG, "%s : Sending BADMETHOD", __func__);
            evhttp_send_error(request, HTTP_BADMETHOD, NULL);
            goto done;
        }
        answer(request);
        return;
    }

    if (STR_EQ(uri_str, "/index/view_question")) {
        if (METHOD(request) != EVHTTP_REQ_GET) {
            LOG(LOG_DEBUG, "%s : Sending BADMETHOD", __func__);
            evhttp_send_error(request, HTTP_BADMETHOD, NULL);
            goto done;
        }
        question(request);
        return;
    }

    LOG(LOG_DEBUG, "%s : Sending NOTFOUND!\n", __func__);
    evhttp_send_error(request, HTTP_NOTFOUND, NULL);
done:
}
