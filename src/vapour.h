/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef WEBSYBIL_VAPOUR_H
#define WEBSYBIL_VAPOUR_H 1

#include "types.h"
#include <stdlib.h>

struct websybil_known_browsers {
    char *name;
    unsigned char sig[kMaxHttpFields];
};

const char *get_websybil_browser_prediction(const char *request, struct websybil_known_browsers *vp, size_t vp_size, int *matching_total, const int threshold);

struct websybil_known_browsers *ld_vapour_from(const char *filepath, size_t *items_total);

void free_vapour_ctx(struct websybil_known_browsers *vp, const size_t items_total);

float get_websybil_success_rate(const int matching_total);

#endif
