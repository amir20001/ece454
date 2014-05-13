#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ece454rpc_types.h"

void init(queue *q) {
	q->front = NULL;
	q->rear = NULL;
}

int isEmpty(queue *q) {
	if (q->front == NULL) {
		return 1;
	} else {
		return 0;
	}
}

void enqueue(queue *q, const char* name, int nparams, fp_type fp) {
	node *n = (node*)malloc(sizeof(node));

	n->name = name;
	n->nparams = nparams;
	n->fp = fp;
	
	n->next = NULL;
	if(!isEmpty(q)) {
		q->rear->next = n;
		q->rear = n;
	} else {
		q->front = n;
		q->rear = n;
	}
	return;
}

void print(queue *q) {
	node *n = q->front;
	while(n != NULL) {
		printf("%s:%d\n",n->name, n->nparams);
		n = n->next;
	}
}

node* find(queue *q, char* name) {
	node *n = q->front;
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
