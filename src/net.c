/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifdef WEBSYBIL_LINUX
#include "linux/net.h"
#endif  //  WEBSYBIL_LINUX

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
