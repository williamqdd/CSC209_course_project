#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "variables.h"


Node *front = NULL;
/* Create and return a new variable node with value using the supplied string and size. */
Node *create_node(char *variable, char *value, Node *next) {
	Node *new_node = malloc(sizeof(Node));
	new_node->variable = (char*) malloc(sizeof(char)*strlen(variable)+1);
	strcpy(new_node->variable, variable);
	new_node->value = (char*)malloc(sizeof(char)*strlen(value)+1);
	strcpy(new_node->value, value);
	new_node->next = next;
	return new_node;
}



int assign(Node **front_ptr, char *variable, char *value) {

	if(*front_ptr == NULL){
		*front_ptr = create_node(variable, value, NULL);
	}
	else{
		insert_end(front_ptr, variable, value);
	}
	return 0;
}

int insert_end(Node **front_ptr, char *variable, char *value){
	Node *curr = *front_ptr;
	while(curr!=NULL){
		if (strcmp(curr->variable,variable)==0){
			free(curr->value);
			curr->value = (char*)malloc(sizeof(char)*strlen(value)+1);
			strcpy(curr->value, value);
			return 0;
		}
		curr = curr->next;
	}
	curr = *front_ptr;
	while(curr->next != NULL){
		curr=curr->next;
	}
	Node *new_node = create_node(variable, value, NULL);
	curr->next=new_node;
	return 0;
}

char *find_value(Node **front_ptr, char *variable) {
	Node *curr =  *front_ptr;
	while (curr!= NULL && strcmp(curr->variable,variable)!=0) {
		curr = curr->next;
	}
	if(curr == NULL) {
		return NULL;
	}
	return curr->value;
}
