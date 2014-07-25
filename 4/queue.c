#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

typedef struct _node {
	int id;
	DIR *dir;
	struct _node *next;
} node;

void print_q(node *n);
node *find(node *n, int id);
bool add_open_dir(DIR* dir, int id);

void print_q(node *n) {
	while (n != NULL) {
		printf("%d:\n", n->id);
		n = n->next;
	}
}

node* find(node *n,int id) {
	node *found = NULL;
	while (n != NULL) {
		//printf("%s==%s\n", name, n->name);
		if (n->id ==id) {
			found = n;
			break;
		}
		n = n->next;
	}

	return found;
}
