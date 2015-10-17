/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "types.h"
#include "vapour.h"
#include "net.h"
#include "http_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

static int g_should_abort = 0;

char *get_option(const char *option, char **argv, const int argc) {
    char temp[255];
    int a = 0;
    int t = 0;
    if (option == NULL || argv == NULL) {
        return NULL;
    }
    memset(temp, 0, sizeof(temp));
    strncpy(temp, "--", sizeof(temp) - 1);
    strncat(temp, option, sizeof(temp) - 1);
    strncat(temp, "=", sizeof(temp) - 1);
    for (t = 0; t < 2; t++) {
        for (a = 0; a < argc; a++) {
            if (strstr(argv[a], temp) == argv[a]) {
                return (&argv[a][0] + strlen(temp));
            }
        }
        temp[strlen(temp)-1] = '\0';
    }
    return NULL;
}

void sigint_watchdog(int sign) {
    g_should_abort = 1;
}

int get_data_from_stdin(struct websybil_known_browsers *vp, size_t vp_size, int *matching_total) {
    FILE *stdpipe = fopen("/dev/stdin", "rb");
    char byte[0x2] = "";
    char inbuf[0xffff] = "";
    const char *browser = NULL;
    int c = 0;
    if (stdpipe == NULL) {
        printf("ERROR: unable to read data from \"/dev/stdin\".\n");
        return 1;
    }
    while (!g_should_abort) {
        memset(inbuf, 0, sizeof(inbuf));
        if (byte[0] == 'G' || byte[0] == 'P' || byte[0] == 'H') {
            strncat(inbuf, byte, sizeof(inbuf) - 1);
        }
        memset(byte, 0, sizeof(byte));
        byte[0] = fgetc(stdpipe);
        c = 0;
        while (!feof(stdpipe) && c < 2) {
            strncat(inbuf, byte, sizeof(inbuf) - 1);
            byte[0] = fgetc(stdpipe);
            c = 0;
            while ((byte[0] == '\n' || byte[0] == '\r') && c < 2) {
                strncat(inbuf, byte, sizeof(inbuf) - 1);
                byte[0] = fgetc(stdpipe);
                c++;
            }
            if (c == 2) {
                fseek(stdpipe, ftell(stdpipe) - 1, SEEK_SET);
            }
        }
        if (inbuf[0] != 0) {
            browser = get_websybil_browser_prediction(inbuf, vp, vp_size, matching_total);
            if (matching_total == NULL) {
                printf(WEBSYBIL_RESULT_MESSAGE, browser);
            } else {
                printf(WEBSYBIL_RESULT_MESSAGE_WITH_SUCCESS_RATE, browser, get_websybil_success_rate(*matching_total));
            }
        }
        usleep(10);
    }
    fclose(stdpipe);
    if (vp != NULL) {
        free_vapour_ctx(vp, vp_size);
    }
    printf("\n");
    return 0;
}

#ifdef WEBSYBIL_LINUX

int get_data_from_wire(const char *iface, struct websybil_known_browsers *vp, size_t vp_size, int *matching_total) {
    int sockfd = new_raw_socket(iface);
    unsigned char buf[0xffff];
    char *get_req = NULL, *src_addr = NULL;
    const char *browser = NULL;
    int buf_size = 0;
    if (sockfd == -1) {
        printf("ERROR: on raw socket creation.\n");
        return 1;
    }
    while (!g_should_abort) {
        buf_size = recvfrom(sockfd, buf, sizeof(buf), NULL, NULL);
        if (buf_size > 0) {
            get_req = parse_http_request_from_wire(buf, buf_size);
            src_addr = get_src_addr_from_pkt(buf, buf_size);
            if (get_req != NULL) {
                browser = get_websybil_browser_prediction(get_req, vp, vp_size, matching_total);
                if (matching_total == NULL) {
                    printf(WEBSYBIL_FROM_WIRE_RESULT_MESSAGE, src_addr, browser);
                } else {
                    printf(WEBSYBIL_RESULT_MESSAGE_WITH_SUCCESS_RATE, browser, get_websybil_success_rate(*matching_total));
                }
                if (src_addr != NULL) {
                    free(src_addr);
                }
                free(get_req);
            }
        }
        usleep(10);
    }
    del_raw_socket(sockfd, iface);
    if (vp != NULL) {
        free_vapour_ctx(vp, vp_size);
    }
    printf("\n");
    return 0;
}

#endif

char *format_request_buffer(const char *request) {
    const char *rp = NULL;
    char *retval = NULL, *rtp = NULL;
    if (request == NULL) {
        return NULL;
    }
    retval = (char *)malloc(strlen(request) + 1);
    rtp = retval;
    memset(retval, 0, strlen(request) + 1);
    for (rp = request; *rp != 0; rp++) {
        if (*rp != '\\') {
            *rtp = *rp;
            rtp++;
        } else {
            rp++;
            switch (*rp) {
                case 'n':
                    *rtp = '\n';
                    rtp++;
                    break;

                case 'r':
                    *rtp = '\r';
                    rtp++;
                    break;

                default:
                    //  INFO(Santiago): just skip.
                    break;
            }
        }
    }
    return retval;
}

int main(int argc, char **argv) {
    size_t vapour_size = 0;
    struct websybil_known_browsers *vapour = NULL;
    const char *option = NULL;
    int matching_total = 0;
    const char *browser = NULL;
    const char *prediction_rate = NULL;
    char *request_buffer = NULL;
    if (get_option("help", argv, argc) != NULL) {
        printf("%s\n%s\n", WEBSYBIL_BANNER, WEBSYBIL_USAGE_BANNER, argv[0]);
        return 1;
    }
    prediction_rate = get_option("prediction-rate", argv, argc);
    option = get_option("vapour-dump", argv, argc);
    if (option != NULL) {
        vapour = ld_vapour_from(option, &vapour_size);
    }
    option = get_option("request-buffer", argv, argc);
    if (option != NULL) {
        request_buffer = format_request_buffer(option);
        if (request_buffer == NULL) {
            printf("ERROR: invalid buffer.\n");
            return 1;
        }
        browser = get_websybil_browser_prediction(request_buffer,
                                                  vapour, vapour_size,
                                                  (prediction_rate != NULL) ? &matching_total : NULL);
        free(request_buffer);
        if (prediction_rate == NULL) {
            printf(WEBSYBIL_RESULT_MESSAGE, browser);
        } else {
            printf(WEBSYBIL_RESULT_MESSAGE_WITH_SUCCESS_RATE, browser, get_websybil_success_rate(matching_total));
        }
        if (vapour != NULL) {
            free_vapour_ctx(vapour, vapour_size);
        }
        return 0;
    }
#ifdef WEBSYBIL_LINUX
    option = get_option("from-wire", argv, argc);
    if (option != NULL) {
        signal(SIGINT, sigint_watchdog);
        signal(SIGTERM, sigint_watchdog);
        return get_data_from_wire(option, vapour, vapour_size, (prediction_rate != NULL) ? &matching_total : NULL);
    }
#endif
    signal(SIGINT, sigint_watchdog);
    signal(SIGTERM, sigint_watchdog);
    return get_data_from_stdin(vapour, vapour_size, (prediction_rate != NULL) ? &matching_total : NULL);
}
