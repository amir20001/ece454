#include "ece454rpc_types.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 4096

return_type make_remote_call(const char *servernameorip,
        const int serverportnumber,
        const char *procedure_name,
        const int nparams,
        ...) {

    int sock;
    struct sockaddr_in server;
    va_list list_pointer;
 
    char buf[BUF_SIZE];
    unsigned int buf_index = 0;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("failed creating socket");
        exit(EXIT_FAILURE);
    }
    if (inet_pton(AF_INET, servernameorip, &(server.sin_addr)) <= 0) {
        struct hostent *lh = gethostbyname(servernameorip);
        if (lh) {
            struct in_addr **addr_list;
            addr_list = (struct in_addr **) lh->h_addr_list;
            if (addr_list[0] != NULL) {
                // use the first ip in the list 
                inet_pton(AF_INET, inet_ntoa(*addr_list[0]), &(server.sin_addr));
            } else {
                //invalid hostname
                exit(EXIT_FAILURE);
            }

        } else {
            printf("invalid name or ip\n");
            exit(EXIT_FAILURE);
        }
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(serverportnumber);
    
    memcpy(buf, &nparams, sizeof (nparams));
    buf_index += sizeof (nparams);
    
    size_t length = strlen(procedure_name);
    memcpy(buf + buf_index, &length, sizeof (length));
    buf_index += sizeof (length);

    strcpy(buf + buf_index, procedure_name);
    buf_index += length;

    va_start(list_pointer, nparams);

    int i;
    for (i = 0; i < nparams; i++) {
        int size = va_arg(list_pointer, int);
        memcpy(buf + buf_index, &size, sizeof (int));
        buf_index += sizeof(int);

        void* valuept = va_arg(list_pointer, void*);
        memcpy(buf + buf_index, valuept, size);
        buf_index += size;
    }
    va_end(list_pointer);

    sendto(sock, buf, sizeof (buf), 0, (struct sockaddr *) &server, sizeof (server));
    int nread;
    char read[BUF_SIZE];
    return_type ret;

    nread = recv(sock, read, BUF_SIZE, 0);
    memcpy(&ret, read, sizeof(ret));
    void* return_value = (void *)malloc(ret.return_size);
    memcpy(return_value, read+sizeof(ret), ret.return_size);
    ret.return_val = return_value;
    close(sock);
    
    return ret;
}
