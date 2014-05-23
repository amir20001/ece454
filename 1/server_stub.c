#include "ece454rpc_types.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>

#define BUF_SIZE 2048

queue *function_list;
arg_type *arg_list;
arg_type *current;

void init_list(int size, void* param) {
    arg_type *ptr = (arg_type*)malloc(sizeof(arg_type));
    ptr->arg_size = size;
    ptr->arg_val = param;
    ptr->next = NULL;
    arg_list = current = ptr;
}

void append(int size, void * param) {
    if (arg_list == NULL) {
        init_list(size, param);
    } else {
        arg_type *p = (arg_type*)malloc(sizeof(arg_type));

        p->arg_size = size;
        p->arg_val = param;
        p->next = NULL;
        
        current->next = p;
        current = p;
    }
}

void launch_server() {
    int sock;
    struct sockaddr_in server, remaddr; //our address, remote address
    socklen_t addrlen = sizeof (remaddr);
    char buf[2048];
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
            int nparams;
            size_t func_name_size;
            int buf_index=0;
            int i=0;
            
            memcpy(&nparams, buf+buf_index, sizeof(nparams));
            buf_index+=sizeof(nparams);
            printf("nparam: %i\n", nparams);
            
            memcpy(&func_name_size, buf+buf_index, sizeof(size_t));
            buf_index+=sizeof(size_t);
            printf("size: %zd\n", func_name_size);
            
            char func_name[func_name_size+1];
            func_name[func_name_size]=0;
            memcpy(func_name,buf+buf_index,func_name_size);
            buf_index+=func_name_size;
            printf("func_name: %s\n",func_name);
            
            for(i = 0; i < nparams; i++) {
                size_t param_size;
                memcpy(&param_size, buf + buf_index, sizeof (size_t));
                buf_index += sizeof (size_t);
                printf("param_size: %zd\n", param_size);
                
                void *param = malloc(param_size);
                memcpy(param, buf + buf_index, param_size);
                buf_index += param_size;
                // TODO do something with this
                
                append(param_size, param);
            }
            
            printf("Received: %s\n", func_name);

            node *n = find(function_list, func_name);
            char t[BUF_SIZE];
            //return_type ret = NULL;

            if (n != NULL) {
                printf("found %s with %d %p\n", n->name, n->nparams, n->fp);
                fp_type func = (n->fp);
                return_type ret = (*func)(nparams, arg_list);
                printf("calc\n");
                //printf("answer: %d, %d\n", ret.return_size, (ret.return_val));
                memcpy(t, &ret, sizeof(ret));
                sendto(sock, t, strlen(t), 0, (struct sockaddr *) &remaddr, addrlen);
            } else {
                printf("not found\n");
                //return_type *f = (return_type*)malloc(sizeof(return_type));
                //f->return_size = 0;
                //f->return_val = (void*)NULL;
                //memcpy(t, ret, sizeof(&ret));
                //sendto(sock, t, strlen(t), 0, (struct sockaddr *) &remaddr, addrlen);
            }
            //printf("answer: %d, %d\n", ret.return_size, (ret.return_val));

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