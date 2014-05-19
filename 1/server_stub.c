#include "ece454rpc_types.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>

queue *function_list;

#define BUF_SIZE 2048

void launch_server() {
    int sock;
    struct sockaddr_in server, remaddr; //our address, remote address
    socklen_t addrlen = sizeof (remaddr);
    char buf[BUF_SIZE];
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

    //printf("%s %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
    printf("%s %d\n", host, ntohs(server.sin_port));

    for (;;) {
        //rc = recv (sock, buf, sizeof(buf), 0);
        rc = recvfrom(sock, buf, sizeof (buf), 0, (struct sockaddr *) &remaddr, &addrlen);
        if (rc > 0) {
            buf[rc] = 0;
            printf("Received: %s\n", buf);
            
            char *sep = ":";
            char *func_name, *token;
            int nparams;
            arg_type *list = NULL;
            arg_type *current = NULL;
            arg_type *ptr = (arg_type*)malloc(sizeof(arg_type));
            
            func_name = strtok(buf, sep);
            nparams = atoi(strtok(NULL, sep));
            
            if (nparams > 0) {
                token = strtok(NULL, sep);
                int size = atoi(token);
                token = strtok(NULL, sep);
                int a = atoi(token);
                void *arg = &a;
                ptr->arg_size = size;
                ptr->arg_val = arg;
                ptr->next = NULL;
            }
            list = current = ptr;
            
            if (nparams > 1) {
                int i;
                for(i = 1; i< nparams; i++) {
                    /* While there are tokens in "string" */
                    token = strtok(NULL, sep);
                    int size = atoi(token);

                    token = strtok(NULL, sep);
                    int a = atoi(token);
                    void *arg = &a;
                    arg_type *p = (arg_type*)malloc(sizeof(arg_type));
                    
                    p->arg_size = size;
                    p->arg_val = arg;
                    p->next = NULL;

                    current->next = p;
                    current = p;
                }
            }
            
            node *n = find(function_list, func_name);

            if (n != NULL) {
                printf("found %s with %d\n", n->name, n->nparams);

                fp_type ff = n->fp;
                return_type ret = (*ff)(nparams, list);

                char b[BUF_SIZE];
                char integer_string[32];
                sprintf(integer_string, "%d", *(int*)(ret.return_val));
                strcpy(b, integer_string);
                printf("sending: %s\n", b);
                sendto(sock, b, strlen(b), 0, (struct sockaddr *) &remaddr, addrlen);
            } else {
                char *ret = "invalid";
                sendto(sock, ret, strlen(ret), 0, (struct sockaddr *) &remaddr, addrlen);
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
    //printf("registering procedure %s\n", procedure_name);
    enqueue(function_list, procedure_name, nparams, fnpointer);

    return true;
}