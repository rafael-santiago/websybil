#include "http_parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static websybil_http_fields_t get_http_field_index(const char *request);

static char *request_normalize(const char *request);

static websybil_http_fields_t get_http_field_index(const char *request) {
    if (request == NULL) {
        return kNone;
    }
    if (strstr(request, "host:") == request) {
        return kHost;
    }
    if (strstr(request, "user-agent:") == request) {
        return kUserAgent;
    }
    if (strstr(request, "accept:") == request) {
        return kAccept;
    }
    if (strstr(request, "accept-language:") == request) {
        return kAcceptLanguage;
    }
    if (strstr(request, "accept-encoding:") == request) {
        return kAcceptEncoding;
    }
    if (strstr(request, "referer:") == request) {
        return kReferer;
    }
    if (strstr(request, "connection:") == request) {
        return kConnection;
    }
    return kNone;
}

static char *request_normalize(const char *request) {
    char *retval = NULL;
    char *rp = NULL;
    size_t sz = 0;
    if (request == NULL) {
        return NULL;
    }
    sz = strlen(request);
    retval = (char *) malloc(sz + 1);
    if (retval == NULL) {
        return NULL;
    }
    memset(retval, 0, sz + 1);
    memcpy(retval, request, sz);
    for (rp = retval; *rp != 0; rp++) {
        *rp = tolower(*rp);
    }
    return retval;
}

websybil_http_fields_vect_t get_http_request_signature(const char *request) {
    char *wreq = request_normalize(request);
    char *wp = NULL;
    size_t f = 0;
    websybil_http_fields_vect_t retval = NULL;
    if (wreq == NULL) {
        return NULL;
    }
    retval = (websybil_http_fields_vect_t) malloc(sizeof(websybil_http_fields_t) * kMaxHttpFields);
    memset(retval, kNone, sizeof(websybil_http_fields_t) * kMaxHttpFields);
    wp = wreq;
    while (*wp != '\n' && *wp != 0) {
        wp++;
    }
    if (*wp == '\n') {
        for (; *wp != 0 && f < kMaxHttpFields; wp++) {
            if (*wp == '\n') {
                wp++;
                retval[f++] = get_http_field_index(wp);
            }
        }
    }
    free(wreq);
    return retval;
}

