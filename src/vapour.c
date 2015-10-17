/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "vapour.h"
#include "http_parser.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static struct websybil_known_browsers browser_signature[] = {
    { "Firefox", {kHost,
                  kUserAgent,
                  kAccept,
                  kAcceptLanguage,
                  kAcceptEncoding,
                  kConnection  }
    },
    {  "Chrome", {kHost,
                  kConnection,
                  kUserAgent,
                  kAccept,
                  kAcceptEncoding,
                  kAcceptLanguage}
    },
    {      "IE", {kAccept,
                  kAcceptLanguage,
                  kAcceptEncoding,
                  kUserAgent,
                  kHost,
                  kConnection}
    }
};

static void get_next_vapour_data_from_vapour_file(struct websybil_known_browsers **vdata, FILE *vp);

static size_t get_vapour_file_item_count(FILE *vp);

#define WEBSYBIL_SIG_TOTAL sizeof(browser_signature) / sizeof(browser_signature[0])

const char *get_websybil_browser_prediction(const char *request, struct websybil_known_browsers *vp, size_t vp_size, int *matching_total) {
    size_t s, matches, f, prior_matches = 0;
    long sig_index = -1;
    struct websybil_known_browsers *vapour = &browser_signature[0];
    size_t vapour_size = WEBSYBIL_SIG_TOTAL;
    websybil_http_fields_vect_t req_sig = get_http_request_signature(request);
    if (vp != NULL) {
        vapour = vp;
        vapour_size = vp_size;
    }
    if (req_sig != NULL) {
        for (s = 0; s < vapour_size; s++) {
            matches = 0;
            for (f = 0; f < kMaxHttpFields; f++) {
                if (req_sig[f] == vapour[s].sig[f]) {
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
    if (matching_total != NULL) {
        *matching_total = prior_matches;
    }
    return (sig_index == -1) ? "(unk)" : vapour[sig_index].name;
}

static size_t get_vapour_file_item_count(FILE *vp) {
    char line[0xffff], *lp = NULL;
    size_t c = 0;
    if (vp == NULL) {
        return 0;
    }
    fseek(vp, 0L, SEEK_SET);
    fgets(line, sizeof(line), vp);
    while (!feof(vp)) {
        memset(line, 0, sizeof(line));
        fgets(line, sizeof(line), vp);
        lp = &line[0];
        while (*lp == '\t' || *lp == ' ') {
            lp++;
        }
        if (*lp != '#' && *lp != '\n' && *lp != '\r' && *lp != 0) {
            c++;
        }
    }
    fseek(vp, 0L, SEEK_SET);
    return c;
}

struct websybil_known_browsers *ld_vapour_from(const char *filepath, size_t *items_total) {
    FILE *vp = NULL;
    size_t signatures_count = 0;
    struct websybil_known_browsers *vdata = NULL, *vd = NULL;
    vp = fopen(filepath, "rb");
    if (vp == NULL) {
        printf("ERROR: unable to read vapour file from \"%s\".\n", filepath);
        return NULL;
    }
    signatures_count = get_vapour_file_item_count(vp);
    if (signatures_count > 0) {
        vdata = (struct websybil_known_browsers *)malloc(signatures_count * sizeof(struct websybil_known_browsers));
        memset(vdata, 0, signatures_count * sizeof(struct websybil_known_browsers));
    }
    if (items_total != NULL) {
        *items_total = signatures_count;
    }
    vd = vdata;
    while (signatures_count-- > 0) {
        get_next_vapour_data_from_vapour_file(&vd, vp);
        vd++;
    }
    fclose(vp);
    return vdata;
}

static void get_next_vapour_data_from_vapour_file(struct websybil_known_browsers **vdata, FILE *vp) {
    char line[0xffff] = "#", *lp = &line[0], *lp_end = &line[0] + sizeof(line);
    char item[0xffff], *ip = NULL, *ip_end = &item[0] + sizeof(item);
    struct websybil_known_browsers *vdata_p = NULL;
    int state = 0, f = 0;
    const int max_state = 2;
    if (vp == NULL || feof(vp) || vdata == NULL) {
        return;
    }
    vdata_p = *vdata;
    while (*lp == '#' && !feof(vp)) {
        memset(line, 0, sizeof(line));
        fgets(line, sizeof(line), vp);
        lp = &line[0];
        while (*lp == ' ' || *lp == '\t' && lp != lp_end) {
            lp++;
        }
    }
    if (*lp != '#') {
        for (state = 0; state < max_state; state++) {
            memset(item, 0, sizeof(item));
            ip = &item[0];
            while (*lp != ' ' && *lp != '\t' && lp != lp_end && ip != ip_end) {
                *ip = (state == 0) ? *lp : tolower(*lp);
                lp++;
                ip++;
            }
            switch (state) {
                case 0:
                    vdata_p->name = (char *)malloc(strlen(item) + 1);
                    strncpy(vdata_p->name, item, strlen(item));
                    break;

                case 1:
                    f = 0;
                    for (ip = &item[0]; *ip != 0 && f < kMaxHttpFields; ip++, f++) {
                        while (*ip == ' ' || *ip == '\t') {
                            ip++;
                        }
                        if (strstr(ip, "host") == ip) {
                            vdata_p->sig[f] = kHost;
                        } else if (strstr(ip, "user-agent") == ip) {
                            vdata_p->sig[f] = kUserAgent;
                        } else if (strstr(ip, "accept-language") == ip) {
                            vdata_p->sig[f] = kAcceptLanguage;
                        } else if (strstr(ip, "accept-encoding") == ip) {
                            vdata_p->sig[f] = kAcceptEncoding;
                        } else if (strstr(ip, "accept") == ip) {
                            vdata_p->sig[f] = kAccept;
                        } else if (strstr(ip, "connection") == ip) {
                            vdata_p->sig[f] = kConnection;
                        } else if (strstr(ip, "none") == ip) {
                            vdata_p->sig[f] = kNone;
                        } else {
                            printf("WARNING: dummy field \"%s\" for \"%s\".\n", ip, vdata_p->name);
                        }
                        while (*ip != ',' && *ip != 0) {
                            ip++;
                        }
                    }
                    break;

                default:
                    //duh!!
                    break;
            }
            while (*lp == ' ' || *lp == '\t' && lp != lp_end) {
                lp++;
            }
        }
    }
}

void free_vapour_ctx(struct websybil_known_browsers *vp, const size_t items_total) {
    size_t t;
    for (t = 0; t < items_total; t++) {
        free(vp[t].name);
    }
    free(vp);
}

float get_websybil_success_rate(const int matching_total) {
    return (((float)matching_total / kMaxHttpFields) * 100);
}
