#ifndef WEBSYBIL_TYPES_H
#define WEBSYBIL_TYPES_H 1

typedef enum {
    kNone = 0,
    kHost,
    kUserAgent,
    kAccept,
    kAcceptLanguage,
    kAcceptEncoding,
    kReferer,
    kConnection,
    kMaxHttpFields
}websybil_http_fields_t;

typedef int *websybil_http_fields_vect_t;

#endif
