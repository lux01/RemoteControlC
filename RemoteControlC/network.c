#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"

#ifdef _WIN32

// Windows headers
#include <winsock2.h>

#elif __linux

// Linux headers
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#endif

void print_system_ips() {
    #ifdef _WIN32
    struct WSAData wsaData;
    char ac[80];
    struct hostent *phe;
	struct in_addr addr;
    int i;

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Couldn't initialise WinSocket 2.2! Can't deduce machine address.");
    } else {
        if(gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
            fprintf(stderr, "Error 0x%x when getting local hostname.\n", WSAGetLastError());
            return;
        }

        phe = gethostbyname(ac);

        if(phe == 0) {
            fprintf(stderr, "Bad hostname lookup.\n");
            return;
        }

        printf("Your system recognises the following IP addresses:\n");
        printf("+-----------------------------------------------+\n");
        for(i = 0; phe->h_addr_list[i] != 0; i++) {
			memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
            printf("| %-45s |\n",	inet_ntoa(addr));
        }
        printf("+-----------------------------------------------+\n");

        WSACleanup();
    }

    #elif __linux

    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if(getifaddrs(&ifaddr) == -1) {
        fprintf(stderr, "getifaddrs failed.\n");
        return ;
    }

    printf("Your system recognises the following IP addresses:\n");
    printf("+-----------------------------------------------+\n");
    for(ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if(ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        if(family == AF_INET || family == AF_INET6) {
            s = getnameinfo(ifa->ifa_addr,
                    (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                    host,
                    NI_MAXHOST,
                    NULL,
                    0,
                    NI_NUMERICHOST
                );
            if(s != 0) {
                fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
            }
            printf("| %-45s |\n", host);
        }
    }
    printf("+-----------------------------------------------+\n");
    freeifaddrs(ifaddr);

    #endif
}
