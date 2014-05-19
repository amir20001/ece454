#include "ece454rpc_types.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#define BUF_SIZE 2048

return_type make_remote_call(const char *servernameorip,
        const int serverportnumber,
        const char *procedure_name,
        const int nparams,
        ...) {
    int sock;
    struct sockaddr_in server;
    struct hostent *host;
    va_list arguments;
    
    va_start ( arguments, nparams);

    char buf[BUF_SIZE];
    char read[BUF_SIZE];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("failed creating socket");
    }
    if (inet_pton(AF_INET, servernameorip, &(server.sin_addr)) <= 0) {
        host = gethostbyname(servernameorip);
        if (host != NULL) {
            struct in_addr **addr_list;
            addr_list = (struct in_addr **) host->h_addr_list;
            //printf("%s\n", inet_ntoa(*addr_list[0]));
            inet_pton(AF_INET, inet_ntoa(*addr_list[0]), &(server.sin_addr));
        } else {
            printf("invalid name or ip");
            exit(EXIT_FAILURE);
        }
    }

    //inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverportnumber);
    
    arg_type *list = NULL;
    arg_type *current = NULL;
    arg_type *ptr = (arg_type*)malloc(sizeof(arg_type));
    ptr->next = NULL;
    int x = 0;
    if (x < nparams) {
        ptr->arg_size = va_arg(arguments, int);
        ptr->arg_val = va_arg(arguments, void*);
    } else {
        printf("no args?\n");
    }
    
    list = current = ptr;
    
    for(x = 1; x < nparams; x++) {
        arg_type *p = (arg_type*)malloc(sizeof(arg_type));

        p->arg_size = va_arg(arguments, int);
        p->arg_val = va_arg(arguments, void*);
        p->next = NULL;
        
        current->next = p;
        current = p;
    }
    va_end(arguments);
    

    strcpy(buf, procedure_name);
    strcat(buf, ":");
    char integer_string[32];
    sprintf(integer_string, "%d", nparams);
    strcat(buf, integer_string);
    
    arg_type *b = list;
    //check that everything is okay
    while(b != NULL) {
        //printf("pr arg %d=%d\n", b->arg_size, *(int *)(b->arg_val));
        sprintf(integer_string, ":%d:", b->arg_size);
        strcat(buf, integer_string);
        sprintf(integer_string, "%d", *(int*)(b->arg_val));
        strcat(buf, integer_string);
        b = b->next;
    }
    
    printf("sending: %s\n", buf);
    
    sendto(sock, buf, strlen(buf), 0, (struct sockaddr *) &server, sizeof (server));
    int nread;

    nread = recv(sock, read, BUF_SIZE, 0);
    read[nread] = 0;
    printf("Received: %s\n", read);

    close(sock);
    return_type ret;
    int res = atoi(read);
    ret.return_val = &res;
    ret.return_size = sizeof(res);

    return ret;
}