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
        browser = get_websybil_browser_prediction(data[t].get_req, NULL, 0, NULL, -1);
        CUTE_ASSERT(browser != NULL);
        CUTE_ASSERT(strcmp(browser, "(unk)") != 0);
        sprintf(msg, "strcmp(browser, \"%s\") != 0 [browser = \"%s\"]", data[t].browser, browser);
        CUTE_CHECK(msg, strcmp(browser, data[t].browser) == 0);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(parse_http_request_from_wire_tests)
    unsigned char *pkt =
        "\x08\x95\x2a\xad\xd6\x4f\x5c\xac\x4c\xaa\xf5\xb5\x08\x00"
        "\x45\x00\x01\xa6\x28\x3b\x40\x00\x80\x06\x42\xc3\xc0\xa8\x01\x05\xc8\xa0\x04\x06"
        "\x04\xe6\x00\x50\xd0\xbf\xb9\x97\x84\x8e\x56\x58\x50\x18\x11\x04\x1d\x3e\x00\x00"
        "GET / HTTP/1.1\r\n"
        "Host: www.nic.br\r\n"
        "Connection: keep-alive\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.6,en;q=0.4\r\n\r\n";
    int pkt_size = 436;
    char *expected = "GET / HTTP/1.1\r\n"
                     "Host: www.nic.br\r\n"
                     "Connection: keep-alive\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
                     "Upgrade-Insecure-Requests: 1\r\n"
                     "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36\r\n"
                     "Accept-Encoding: gzip, deflate, sdch\r\n"
                     "Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.6,en;q=0.4\r\n\r\n";
    char *retval = parse_http_request_from_wire(pkt, pkt_size);
    CUTE_ASSERT(retval != NULL);
    CUTE_ASSERT(strcmp(retval, expected) == 0);
    free(retval);
    pkt = "\x08\x95\x2a\xad\xd6\x4f\x5c\xac\x4c\xaa\xf5\xb5\x08\x01"
          "\x45\x00\x01\xa6\x28\x3b\x40\x00\x80\x06\x42\xc3\xc0\xa8\x01\x05\xc8\xa0\x04\x06"
          "\x04\xe6\x00\x50\xd0\xbf\xb9\x97\x84\x8e\x56\x58\x50\x18\x11\x04\x1d\x3e\x00\x00"
          "GET / HTTP/1.1\r\n"
          "Host: www.nic.br\r\n"
          "Connection: keep-alive\r\n"
          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
          "Upgrade-Insecure-Requests: 1\r\n"
          "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36\r\n"
          "Accept-Encoding: gzip, deflate, sdch\r\n"
          "Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.6,en;q=0.4\r\n\r\n";
    retval = parse_http_request_from_wire(pkt, pkt_size);
    CUTE_ASSERT(retval == NULL);
    pkt = "\x08\x95\x2a\xad\xd6\x4f\x5c\xac\x4c\xaa\xf5\xb5\x08\x00"
          "\x45\x00\x01\xa6\x28\x3b\x40\x00\x80\x82\x42\xc3\xc0\xa8\x01\x05\xc8\xa0\x04\x06"
          "\x04\xe6\x00\x50\xd0\xbf\xb9\x97\x84\x8e\x56\x58\x50\x18\x11\x04\x1d\x3e\x00\x00"
          "GET / HTTP/1.1\r\n"
          "Host: www.nic.br\r\n"
          "Connection: keep-alive\r\n"
          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
          "Upgrade-Insecure-Requests: 1\r\n"
          "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36\r\n"
          "Accept-Encoding: gzip, deflate, sdch\r\n"
          "Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.6,en;q=0.4\r\n\r\n";
    retval = parse_http_request_from_wire(pkt, pkt_size);
    CUTE_ASSERT(retval == NULL);
    pkt = "\x08\x95\x2a\xad\xd6\x4f\x5c\xac\x4c\xaa\xf5\xb5\x08\x00"
          "\x95\x00\x01\xa6\x28\x3b\x40\x00\x80\x06\x42\xc3\xc0\xa8\x01\x05\xc8\xa0\x04\x06"
          "\x04\xe6\x00\x50\xd0\xbf\xb9\x97\x84\x8e\x56\x58\x50\x18\x11\x04\x1d\x3e\x00\x00"
          "GET / HTTP/1.1\r\n"
          "Host: www.nic.br\r\n"
          "Connection: keep-alive\r\n"
          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
          "Upgrade-Insecure-Requests: 1\r\n"
          "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36\r\n"
          "Accept-Encoding: gzip, deflate, sdch\r\n"
          "Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.6,en;q=0.4\r\n\r\n";
    retval = parse_http_request_from_wire(pkt, pkt_size);
    CUTE_ASSERT(retval == NULL);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(websybil_tests_entry)
    CUTE_RUN_TEST(http_parsing_tests);
    CUTE_RUN_TEST(vapour_tests);
#ifdef WEBSYBIL_LINUX
    CUTE_RUN_TEST(parse_http_request_from_wire_tests);
#endif
CUTE_TEST_CASE_END

CUTE_MAIN(websybil_tests_entry);
