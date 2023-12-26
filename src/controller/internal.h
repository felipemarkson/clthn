#ifndef INTERNAL_H
#define INTERNAL_H

#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/http.h>
#include <event2/keyvalq_struct.h>
#include <string.h>
#include <sys/queue.h>

#include "utils.h"

#define STR_START_WITH(cstr, token) (strncmp((cstr), (token), strlen(token)) == 0)
#define STR_EQ(cstr, token) (strcmp((cstr), (token)) == 0)
#define METHOD(request) evhttp_request_get_command(request)

enum log_type {
    LOG_INFO,
    LOG_DEBUG,
    LOG_ERROR
};

#ifdef DEBUG
#define LOG(logtype, ...)                                     \
    do {                                                      \
        switch ((logtype)) {                                  \
            case LOG_INFO: {                                  \
                fprintf(stdout, __VA_ARGS__);                 \
                break;                                        \
            }                                                 \
            case LOG_DEBUG: {                                 \
                fprintf(stdout, __VA_ARGS__);                 \
                break;                                        \
            }                                                 \
            case LOG_ERROR: {                                 \
                fprintf(stderr, __VA_ARGS__);                 \
                fflush(stderr);                               \
                break;                                        \
            }                                                 \
            default: {                                        \
                fprintf(stderr, "ERROR: INVALID LOG TYPE\n"); \
                fflush(stderr);                               \
                break;                                        \
            }                                                 \
        }                                                     \
    } while (0)
#else
#define LOG(logtype, ...)                                     \
    do {                                                      \
        switch ((logtype)) {                                  \
            case LOG_INFO: {                                  \
                fprintf(stdout, __VA_ARGS__);                 \
                break;                                        \
            }                                                 \
            case LOG_DEBUG: {                                 \
                break;                                        \
            }                                                 \
            case LOG_ERROR: {                                 \
                fprintf(stderr, __VA_ARGS__);                 \
                fflush(stderr);                               \
                break;                                        \
            }                                                 \
            default: {                                        \
                fprintf(stderr, "ERROR: INVALID LOG TYPE\n"); \
                fflush(stderr);                               \
                break;                                        \
            }                                                 \
        }                                                     \
    } while (0)
#endif  // DEBUG

#define ASSERT(check, code_on_error)                                                      \
    do {                                                                                  \
        if (!(check)) {                                                                   \
            evhttp_send_error(request, (code_on_error), NULL);                            \
            LOG(LOG_ERROR, "ERROR on %s:%d Caused by: %s\n", __FILE__, __LINE__, #check); \
            goto done;                                                                    \
        }                                                                                 \
    } while (0)

#endif