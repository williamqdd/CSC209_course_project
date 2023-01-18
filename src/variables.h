#ifndef __VARIABLES_H__
#define __VARIABLES_H__


typedef struct node {
        char *variable;
	char *value;
	struct node *next;
} Node;


Node *front;
/* Create and return a new variable node with value using the supplied string and size. */
Node *create_node(char *variable, char *value, Node *next);

int assign(Node **front_ptr, char *variable, char *value);

int insert_end(Node **front_ptr, char *variable, char *value);

char *find_value(Node **front_ptr, char *variable);
#endif
