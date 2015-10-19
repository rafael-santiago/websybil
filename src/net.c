/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "net.h"
#ifdef WEBSYBIL_LINUX
#include "linux/net.h"
#endif  //  WEBSYBIL_LINUX
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int new_raw_socket(const char *iface) {
#ifdef WEBSYBIL_LINUX
    return new_linrsk(iface);
#else
    return -1;
#endif
}

void del_raw_socket(int socket, const char *iface) {
#ifdef WEBSYBIL_LINUX
    del_linrsk(socket, iface);
#endif
}

char *get_src_addr_from_pkt(const char *pkt, const int pkt_size) {
    char *retval = NULL;
    if (pkt == NULL || pkt_size <= 14) {
        return NULL;
    }
    retval = (char *) malloc(100);
    memset(retval, 0, 100);
    switch (((*(pkt + 14)) & 0xf0) >> 4) {
        case 0x4:
            if (pkt + 15 >= pkt + pkt_size) {
                free(retval);
                return NULL;
            }
            sprintf(retval, "%d.%d.%d.%d", *(unsigned char *)(pkt + 14 + 12), *(unsigned char *)(pkt + 14 + 13), *(unsigned char *)(pkt + 14 + 14), *(unsigned char *)(pkt + 14 + 15));
            break;

        case 0x6:
            if (pkt + 27 >= pkt + pkt_size) {
                free(retval);
                return NULL;
            }
            sprintf(retval, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x:%.2x:%.2x"
                            "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", *(unsigned char *)(pkt + 14 + 12),
                                                                       *(unsigned char *)(pkt + 14 + 13),
                                                                       *(unsigned char *)(pkt + 14 + 14),
                                                                       *(unsigned char *)(pkt + 14 + 15),
                                                                       *(unsigned char *)(pkt + 14 + 16),
                                                                       *(unsigned char *)(pkt + 14 + 17),
                                                                       *(unsigned char *)(pkt + 14 + 18),
                                                                       *(unsigned char *)(pkt + 14 + 19),
                                                                       *(unsigned char *)(pkt + 14 + 20),
                                                                       *(unsigned char *)(pkt + 14 + 21),
                                                                       *(unsigned char *)(pkt + 14 + 22),
                                                                       *(unsigned char *)(pkt + 14 + 23),
                                                                       *(unsigned char *)(pkt + 14 + 24),
                                                                       *(unsigned char *)(pkt + 14 + 25),
                                                                       *(unsigned char *)(pkt + 14 + 26),
                                                                       *(unsigned char *)(pkt + 14 + 27));
            break;

        default:
            free(retval);
            return NULL;
            break;
    }
    return retval;
}