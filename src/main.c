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

int get_data_from_stdin() {
    FILE *stdpipe = fopen("/dev/stdin", "rb");
    char inbuf[0xffff] = "";
    if (stdpipe == NULL) {
        printf("ERROR: unable to read data from \"/dev/stdin\".\n");
        return 1;
    }
    while (!g_should_abort) {
        memset(inbuf, 0, sizeof(inbuf));
        if (fgets(inbuf, sizeof(inbuf), stdpipe) != NULL) {
            printf(WEBSYBIL_RESULT_MESSAGE, get_websybil_browser_prediction(inbuf));
        }
        usleep(10);
    }
    fclose(stdpipe);
    printf("\n");
    return 0;
}

#ifdef WEBSYBIL_LINUX

int get_data_from_wire(const char *iface) {
    int sockfd = new_raw_socket(iface);
    unsigned char buf[0xffff];
    char *get_req = NULL;
    int buf_size = 0;
    if (sockfd == -1) {
        printf("ERROR: on raw socket creation.\n");
        return 1;
    }
    while (!g_should_abort) {
        buf_size = recvfrom(sockfd, buf, sizeof(buf), NULL, NULL);
        if (buf_size > 0) {
            get_req = parse_http_request_from_wire(buf, buf_size);
            if (get_req != NULL) {
                printf(WEBSYBIL_RESULT_MESSAGE, get_websybil_browser_prediction(get_req));
                free(get_req);
            }
        }
        usleep(10);
    }
    del_raw_socket(sockfd, iface);
    printf("\n");
    return 0;
}

#endif

int main(int argc, char **argv) {
    const char *option = NULL;
    option = get_option("request-buffer", argv, argc);
    if (option != NULL) {
        printf(WEBSYBIL_RESULT_MESSAGE, get_websybil_browser_prediction(option));
        return 0;
    }
#ifdef WEBSYBIL_LINUX
    option = get_option("from-wire", argv, argc);
    if (option != NULL) {
        signal(SIGINT, sigint_watchdog);
        signal(SIGTERM, sigint_watchdog);
        return get_data_from_wire(option);
    }
#endif
    if (get_option("help", argv, argc) != NULL) {
        printf("%s\n%s\n", WEBSYBIL_BANNER, WEBSYBIL_USAGE_BANNER, argv[0]); 
        return 1;
    }
    signal(SIGINT, sigint_watchdog);
    signal(SIGTERM, sigint_watchdog);
    return get_data_from_stdin();
}
