#ifndef WEBSYBIL_TYPES_H
#define WEBSYBIL_TYPES_H 1

typedef enum {
    kNone = 0,
    kHost,
    kUserAgent,
    kAccept,
    kAcceptLanguage,
    kAcceptEncoding,
    kConnection,
    kMaxHttpFields = kConnection
}websybil_http_fields_t;

typedef int *websybil_http_fields_vect_t;

#define WEBSYBIL_RESULT_MESSAGE "The request seems to have been generated by %s.\n"

#define WEBSYBIL_RESULT_MESSAGE_WITH_SUCCESS_RATE "The request seems to have been generated by %s. (%.f%%)\n"

#define WEBSYBIL_BANNER "websybil 0.0.1"

#define WEBSYBIL_FROM_WIRE_RESULT_MESSAGE "The host %s seems to be using %s.\n"

#define WEBSYBIL_FROM_WIRE_RESULT_MESSAGE_WITH_SUCCESS_RATE "The host %s seems to be using %s. (%.f%%)\n"

#ifdef WEBSYBIL_LINUX

#define WEBSYBIL_FROM_WIRE_USAGE_BANNER "|--from-wire=<iface-name>"

#else

#define WEBSYBIL_FROM_WIRE_USAGE_BANNER "\0"

#endif

#define WEBSYBIL_USAGE_BANNER "use: %s --request-buffer=<string-buffer>" WEBSYBIL_FROM_WIRE_USAGE_BANNER" [--prediction-rate --prediction-threshold=<number> --vapour-pipe=<filepath>]"

#endif
