/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "native_net.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <string.h>

static int get_iface_index(const char *iface);

static int promisc_mode_on(const char *iface);

static int promisc_mode_off(const char *iface);

static int promisc_mode_on(const char *iface) {
    struct ifreq ifr;
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return 0;
    }
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) == -1) {
        close(sockfd);
        return 0;
    }
    ifr.ifr_flags |= IFF_PROMISC;
    if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) == -1) {
        close(sockfd);
        return 0;
    }
    close(sockfd);
    return 1;
}

static int promisc_mode_off(const char *iface) {
    struct ifreq ifr;
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return 0;
    }
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) == -1) {
        close(sockfd);
        return 0;
    }
    ifr.ifr_flags &= ~(IFF_PROMISC);
    if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) == -1) {
        close(sockfd);
        return 0;
    }
    close(sockfd);
    return 1;

}

static int get_iface_index(const char *iface) {
    struct ifreq ifr;
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return -1;
    }
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) != 0) {
        ifr.ifr_ifindex = -1;
    }
    close(sockfd);
    return ifr.ifr_ifindex;
}


int new_linrsk(const char *iface) {
    int sockfd = -1;
    struct timeval tm;
    if (promisc_mode_on(iface)) {
        sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        memset(&tm, 0, sizeof(tm));
        tm.tv_sec = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(tm));
        struct sockaddr_ll sll;
        memset(&sll, 0, sizeof(sll));
        sll.sll_family = AF_PACKET;
        sll.sll_protocol = htons(ETH_P_ALL);
        sll.sll_ifindex = get_iface_index(iface);
        if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) != 0) {
            del_linrsk(sockfd, iface);
            return -1;
        }
    }
    return sockfd;
}

void del_linrsk(int sockfd, const char *iface) {
    close(sockfd);
    promisc_mode_off(iface);
}

