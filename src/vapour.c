/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "vapour.h"
#include "types.h"
#include "http_parser.h"
#include <stdlib.h>

struct websybil_known_browsers {
    const char *name;
    unsigned char sig[kMaxHttpFields];
};

static struct websybil_known_browsers browser_signature[] = {
    { "Firefox", {kHost,
                  kUserAgent,
                  kAccept,
                  kAcceptLanguage,
                  kAcceptEncoding,
                  kReferer,
                  kConnection  }
    },
    {  "Chrome", {kHost,
                  kConnection,
                  kUserAgent,
                  kAccept,
                  kReferer,
                  kAcceptEncoding,
                  kAcceptLanguage}
    },
    {      "IE", {kAccept,
                  kAcceptLanguage,
                  kReferer,
                  kAcceptEncoding,
                  kUserAgent,
                  kHost,
                  kConnection}
    }
};

#define WEBSYBIL_SIG_TOTAL sizeof(browser_signature) / sizeof(browser_signature[0])

const char *get_websybil_browser_prediction(const char *request) {
    size_t s, matches, f, prior_matches = 0;
    long sig_index = -1;
    websybil_http_fields_vect_t req_sig = get_http_request_signature(request);
    if (req_sig != NULL) {
        for (s = 0; s < WEBSYBIL_SIG_TOTAL; s++) {
            matches = 0;
            for (f = 0; f < kMaxHttpFields; f++) {
                if (req_sig[f] == browser_signature[s].sig[f]) {
                    matches++;
                }
            }
            if (matches > prior_matches) {
                prior_matches = matches;
                sig_index = s;
            }
        }
        free(req_sig);
    }
    return (sig_index == -1) ? "(unk)" : browser_signature[sig_index].name;
}
