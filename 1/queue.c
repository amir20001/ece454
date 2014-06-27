#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "ece454rpc_types.h"

void print_q(node *n) {
    while(n != NULL) {
        printf("%s:%d\n",n->name, n->nparams);
        n = n->next;
    }
}

node* find(node *n, char* name) {
    node *found = NULL;
    while(n != NULL) {
        //printf("%s==%s\n", name, n->name);
        if(strcmp(name, n->name) == 0) {
                found = n;
                break;
        }
        n = n->next;
    }

    return found;
}
