/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef WEBSYBIL_LINUX_NATIVE_NET_H
#define WEBSYBIL_LINUX_NATIVE_NET_H 1

int new_linrsk(const char *iface);

void del_linrsk(int sockfd, const char *iface);

#endif
