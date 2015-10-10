#ifndef WEBSYBIL_HTTP_PARSER_H
#define WEBSYBIL_HTTP_PARSER_H 1

#include "types.h"

websybil_http_fields_vect_t get_http_request_signature(const char *request);

#endif
