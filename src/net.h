/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef WEBSYBIL_NET_H
#define WEBSYBIL_NET_H 1

int new_raw_socket(const char *iface);

void del_raw_socket(int socket, const char *iface);

#endif
