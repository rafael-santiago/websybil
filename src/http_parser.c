/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
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
                retval[f] = get_http_field_index(wp);
                if (retval[f] != kNone) {
                    f++;
                }
            }
        }
    }
    free(wreq);
    return retval;
}

char *parse_http_request_from_wire(const unsigned char *buf, const int buf_size) {
    const unsigned char *datagram = NULL;
    const unsigned char *tcp_chunk = NULL;
    const unsigned char *tcp_payload = NULL;
    const unsigned char *buf_end = NULL;
    char *retval = NULL;
    int data_offset = 0;
    if (buf_size <= 14) {
        return NULL;
    }
    if (*(buf + 12) != 0x08 || *(buf + 13) != 0x00) {
        return NULL;
    }
    datagram = buf + 14;
    switch (((*datagram) & 0xf0) >> 4) {
        case 0x4:
            if (*(datagram + 9) != 0x6) {
                return NULL;
            }
            tcp_chunk = datagram + (((*datagram) & 0x0f) * 4);
            break;

        case 0x6:
            if (*(datagram + 6) != 0x6) {
                return NULL;
            }
            tcp_chunk = datagram + 40;
            break;

        default:
            return NULL;
    }
    data_offset = 4 * ((*(tcp_chunk + 12) & 0xf0) >> 4);
    if (data_offset >= buf_size) {
        return NULL;
    }
    tcp_payload = tcp_chunk + data_offset;
    if (tcp_payload != NULL && (strstr(tcp_payload, "GET /") == (char *)tcp_payload  ||
                                strstr(tcp_payload, "HEAD /") == (char *)tcp_payload ||
                                strstr(tcp_payload, "POST /") == (char *)tcp_payload)) {
        buf_end = buf + buf_size;
        retval = (char *) malloc(buf_end - tcp_payload + 1);
        memset(retval, 0, buf_end - tcp_payload + 1);
        memcpy(retval, tcp_payload, buf_end - tcp_payload);
    }
    return retval;
}
