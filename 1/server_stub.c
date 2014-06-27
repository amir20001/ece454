#include "ece454rpc_types.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <stdbool.h>

#include <net/if.h>

#define BUF_SIZE 4096

node *function_list;

arg_type *arg_list;
arg_type *current;

void append(size_t size, void * param) {
    arg_type *p = (arg_type*)malloc(sizeof(arg_type));
    p->arg_size = size;
    p->arg_val = (void*)malloc(size);
    memcpy(p->arg_val, param, size);
    p->next = NULL;

    if (arg_list == NULL) {
        arg_list = current = p;
    } else {
        current->next = p;
    }
    current = p;
}

void launch_server() {
    int sock;
    struct sockaddr_in server, remaddr; //our address, remote address
    socklen_t addrlen = sizeof (remaddr);
    char buf[BUF_SIZE];
    int rc;

    arg_list = NULL;

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
        
        if ((strcmp("lo", ifa->ifa_name) == 0) || 
          !(ifa->ifa_flags & (IFF_RUNNING)))
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
            if (strcmp(ifa->ifa_name, "eth0") == 0 ||
                    strcmp(ifa->ifa_name, "lo") == 0) {
                break;
            }
        }
    }
    freeifaddrs(ifaddr);

    printf("%s %d\n", host, ntohs(server.sin_port));

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
                int param_size;
                memcpy(&param_size, buf + buf_index, sizeof(int));
                buf_index += sizeof(int);
                
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

            //free stuff up here
            buf_index = 0;
            arg_type *arg = arg_list;
            while(arg != NULL) {
                arg_type *tmp = arg;
                arg = arg->next;
                free(tmp);
            }
            arg_list = current = NULL;
        }
    }
}

bool register_procedure(const char *procedure_name,
        const int nparams,
        fp_type fnpointer) {

    if (procedure_name == NULL) {
        return false;
    }

    node *cur_node = (node *)malloc(sizeof(node));
    cur_node->next = NULL;
    cur_node->name = (char*)calloc(strlen(procedure_name)+1, sizeof(char));
    strcpy((cur_node->name), procedure_name);
    cur_node->nparams = nparams;
    cur_node->fp = fnpointer;
    cur_node->next = function_list;

    function_list = cur_node;

    return true;
}
