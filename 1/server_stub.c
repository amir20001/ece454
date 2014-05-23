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
        exit(EXIT_FAILURE);
    }
 
    server.sin_family = AF_INET;
    server.sin_port = 0;
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (mybind(sock, &server) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
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
            if (n==2) {
                break;
            }
        }
    }
    freeifaddrs(ifaddr);

    //printf("running udp server on %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
    printf("running udp server on %s:%d\n", host, ntohs(server.sin_port));

    //loop forever
    for (;;) {
        //check to see if we are sent any information
        rc = recvfrom(sock, buf, sizeof (buf), 0, (struct sockaddr *) &remaddr, &addrlen);
        if (rc > 0) {
            int nparams;
            size_t func_name_size;
            int buf_index=0;
            int i=0;
            
            //retrieve the number of parameters
            memcpy(&nparams, buf+buf_index, sizeof(nparams));
            buf_index+=sizeof(nparams);
            
            //retrieve the size of the function name
            memcpy(&func_name_size, buf+buf_index, sizeof(size_t));
            buf_index+=sizeof(size_t);
            
            //retrieve the function name
            char func_name[func_name_size+1];
            func_name[func_name_size]=0;
            memcpy(func_name,buf+buf_index,func_name_size);
            buf_index+=func_name_size;
            
            //get all the parameters and add them to the arg_type linked list
            for(i = 0; i < nparams; i++) {
                size_t param_size;
                memcpy(&param_size, buf + buf_index, sizeof (size_t));
                buf_index += sizeof (size_t);
                
                void *param = malloc(param_size);
                memcpy(param, buf + buf_index, param_size);
                buf_index += param_size;
                
                //append the parameters to the linked list
                append(param_size, param);
            }

            //check to see if we can find the function based on name
            node *n = find(function_list, func_name);
            char t[BUF_SIZE];
            return_type ret;

            if (n != NULL) {
                fp_type func = (n->fp);
                ret = (*func)(nparams, arg_list);
                memcpy(t, &ret, sizeof(ret));
                memcpy(t+sizeof(ret), ret.return_val, ret.return_size);
                sendto(sock, t, sizeof(t), 0, (struct sockaddr *) &remaddr, addrlen);
            } else {
                ret.return_val = NULL;
                ret.return_size = 0;
                memcpy(t, &ret, sizeof(ret));
                sendto(sock, t, strlen(t), 0, (struct sockaddr *) &remaddr, addrlen);
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
    enqueue(function_list, procedure_name, nparams, fnpointer);
    return true;
}