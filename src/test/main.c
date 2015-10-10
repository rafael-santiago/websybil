#include "../http_parser.h"
#include <cutest.h>

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

CUTE_TEST_CASE(websybil_tests_entry)
    CUTE_RUN_TEST(http_parsing_tests);
CUTE_TEST_CASE_END

CUTE_MAIN(websybil_tests_entry);
