#include "ece454rpc_types.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024

return_type make_remote_call(const char *servernameorip,
        const int serverportnumber,
        const char *procedure_name,
        const int nparams,
        ...) {
    return_type ret;

    int sock;
    struct sockaddr_in server;

    char buf[BUF_SIZE];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("failed creating socket");
    }
    if (inet_pton(AF_INET, servernameorip, &(server.sin_addr)) <= 0) {
        struct hostent *lh = gethostbyname(servernameorip);
        if (lh) {
            puts(lh->h_name);
            struct in_addr **addr_list;
            addr_list = (struct in_addr **) lh->h_addr_list;
            if (addr_list[0] != NULL) {
                // use the first ip in the list 
              printf( "%s",inet_ntoa(*addr_list[0]));
//                printf(inet_ntoa(*addr_list[0]));
//                printf("\n/n")
                inet_pton(AF_INET, inet_ntoa(*addr_list[0]), &(server.sin_addr));
            } else {
                //invalid hostname
            }

        } else
            printf("invalid name or ip");
    }

    //inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverportnumber);

    char * func = "fds";

    sendto(sock, func, strlen(func), 0, (struct sockaddr *) &server, sizeof (server));
    int nread;

    nread = recv(sock, buf, BUF_SIZE, 0);
    buf[nread] = 0;
    printf("Received: %s\n", buf);

    close(sock);
    return ret;
}