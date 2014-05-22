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

#define BUF_SIZE 2048

return_type make_remote_call(const char *servernameorip,
        const int serverportnumber,
        const char *procedure_name,
        const int nparams,
        ...) {

    int sock;
    struct sockaddr_in server;
    va_list listPointer;
 
    char buf[BUF_SIZE];
    unsigned int buf_index = 0;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("failed creating socket");
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
            }

        } else
            printf("invalid name or ip\n");
    }

    //inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverportnumber);
    
    memcpy(buf, &nparams, sizeof (nparams));
    buf_index += sizeof (nparams);
    
    size_t length = strlen(procedure_name);
    memcpy(buf + buf_index, &length, sizeof (length));
    buf_index += sizeof (length);

    strcpy(buf + buf_index, procedure_name);
    buf_index += length;

    va_start(listPointer, nparams);
    printf("%i parameters\n", nparams);
   
    int i=0;
    for (i = 0; i < nparams; i++) {
        size_t size = va_arg(listPointer, size_t);
        memcpy(buf + buf_index, &size, sizeof (size));
        buf_index += sizeof (size);

        printf("size: %i\n", size);
        void* valuept = va_arg(listPointer, void*);
        memcpy(buf + buf_index, valuept, size);
        buf_index += size;
    }

    sendto(sock, buf, sizeof (buf), 0, (struct sockaddr *) &server, sizeof (server));
    int nread;
    char read[BUF_SIZE];
    return_type ret;

    nread = recv(sock, read, BUF_SIZE, 0);
    //read[nread] = 0;
    memcpy(&ret, read, nread);
    close(sock);
    
    return ret;
}