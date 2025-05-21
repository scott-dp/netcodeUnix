#ifndef SOCKADDR_IN_COMPARATOR_H
#define SOCKADDR_IN_COMPARATOR_H

#include <winsock2.h>

/**
 * returns 0 if the to sockets have the same ip and port, meaning equality
 */
struct sockaddr_in_comparator {
    bool operator()(const sockaddr_in& a, const sockaddr_in& b) const {
        if (a.sin_addr.s_addr != b.sin_addr.s_addr)
            return a.sin_addr.s_addr < b.sin_addr.s_addr;
        return a.sin_port < b.sin_port;
    }
};

#endif // SOCKADDR_IN_COMPARATOR_H
