/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "../http_parser.h"
#include "../vapour.h"
#include <cutest.h>
#include <stdio.h>
#include <stdlib.h>

CUTE_TEST_CASE(http_parsing_tests)
    char *get_req = "GET /duh.html\r\n"
                    "Host: www.duh.com\r\n"
                    "User-Agent: Godzilla/Googolplex+1 (Windows NT + Plan9 + FreeBSD + Linux + Android + Whatever)\r\n"
                    "Accept: image/png,image/*;q=0.8,*/*;q=0.5\r\n"
                    "Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
                    "Accept-Encoding: gzip, deflate\r\n"
                    "Referer: http://www.nowhere.org/\r\n"
                    "Connection: keep-alive\r\n\r\n";
    websybil_http_fields_vect_t rsig = get_http_request_signature(get_req);
    websybil_http_fields_t expected[kMaxHttpFields] = { kHost,
                                                        kUserAgent,
                                                        kAccept,
                                                        kAcceptLanguage,
                                                        kAcceptEncoding,
                                                        kReferer,
                                                        kConnection };
    size_t f = 0;
    CUTE_ASSERT(rsig != NULL);
    for (f = 0; f < kMaxHttpFields; f++) {
        CUTE_ASSERT(rsig[f] == expected[f]);
    }
    free(rsig);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(vapour_tests)
    struct test_data {
        char *browser;
        char *get_req;
    };
    struct test_data data[] = {
        {"Firefox", "GET /duh.html\r\n"
                    "Host: www.duh.com\r\n"
                    "User-Agent: Godzilla/Googolplex+1 (Windows NT + Plan9 + FreeBSD + Linux + Android + Whatever)\r\n"
                    "Accept: image/png,image/*;q=0.8,*/*;q=0.5\r\n"
                    "Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
                    "Accept-Encoding: gzip, deflate\r\n"
                    "Referer: http://www.nowhere.org/\r\n"
                    "Connection: keep-alive\r\n\r\n"},
        { "Chrome", "GET /duh.html\r\n"
                    "Host: www.duh.com\r\n"
                    "Connection: keep-alive\r\n"
                    "User-Agent: Godzilla/Googolplex+1 (Windows NT + Plan9 + FreeBSD + Linux + Android + Whatever)\r\n"
                    "Accept: image/png,image/*;q=0.8,*/*;q=0.5\r\n"
                    "Referer: http://www.nowhere.org/\r\n"
                    "Accept-Encoding: gzip, deflate\r\n"
                    "Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.5,en;q=0.3\r\n\r\n"},
        {     "IE", "GET /duh.html\r\n"
                    "Accept: image/png,image/*;q=0.8,*/*;q=0.5\r\n"
                    "Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
                    "Referer: http://www.nowhere.org/\r\n"
                    "Accept-Encoding: gzip, deflate\r\n"
                    "User-Agent: Godzilla/Googolplex+1 (Windows NT + Plan9 + FreeBSD + Linux + Android + Whatever)\r\n"
                    "Host: www.duh.com\r\n"
                    "Connection: keep-alive\r\n\r\n"}
    };
    size_t test_data_size = sizeof(data) / sizeof(data[0]);
    size_t t = 0;
    const char *browser = NULL;
    static char msg[1024] = "";
    for (t = 0; t < test_data_size; t++) {
        browser = get_websybil_browser_prediction(data[t].get_req);
        CUTE_ASSERT(browser != NULL);
        CUTE_ASSERT(strcmp(browser, "(unk)") != 0);
        sprintf(msg, "strcmp(browser, \"%s\") != 0 [browser = \"%s\"]", data[t].browser, browser);
        CUTE_CHECK(msg, strcmp(browser, data[t].browser) == 0);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(websybil_tests_entry)
    CUTE_RUN_TEST(http_parsing_tests);
    CUTE_RUN_TEST(vapour_tests);
CUTE_TEST_CASE_END

CUTE_MAIN(websybil_tests_entry);
