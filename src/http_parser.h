/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef WEBSYBIL_HTTP_PARSER_H
#define WEBSYBIL_HTTP_PARSER_H 1

#include "types.h"

websybil_http_fields_vect_t get_http_request_signature(const char *request);

#endif
