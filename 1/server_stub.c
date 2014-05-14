#include "ece454rpc_types.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>

queue *function_list;

void launch_server() {
    int sock;
    struct sockaddr_in server, remaddr; //our address, remote address
    socklen_t addrlen = sizeof (remaddr);
    char buf[1024];
    int rc;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("failed creating socket");
    }

    server.sin_family = AF_INET;
    server.sin_port = 0;
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (mybind(sock, &server) == -1) {
        perror("bind failed");
    }

    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
    }

    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        /* For an AF_INET* interface address, display the address */
        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr,
                    sizeof (struct sockaddr_in),
                    host, NI_MAXHOST,
                    NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            //printf("\t\taddress: <%s>\n", host);
            break;
        }
    }
    freeifaddrs(ifaddr);

    printf("running udp server on %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
    printf("running udp server on %s:%d\n", host, ntohs(server.sin_port));

    for (;;) {
        //rc = recv (sock, buf, sizeof(buf), 0);
        rc = recvfrom(sock, buf, sizeof (buf), 0, (struct sockaddr *) &remaddr, &addrlen);
        if (rc > 0) {
            buf[rc] = 0;
            printf("Received: %s\n", buf);

            node *n = find(function_list, buf);

            if (n != NULL) {
                printf("found\n");
                sendto(sock, buf, strlen(buf), 0, (struct sockaddr *) &remaddr, addrlen);
            } else {
                sendto(sock, "none", 4, 0, (struct sockaddr *) &remaddr, addrlen);
            }
        }
    }
}

bool register_procedure(const char *procedure_name,
        const int nparams,
        fp_type fnpointer) {

    if (function_list == NULL) {
        function_list = (queue*) malloc(sizeof (queue));
        init(function_list);
    }
    printf("registering procedure %s\n", procedure_name);
    enqueue(function_list, procedure_name, nparams, fnpointer);

    return true;
}