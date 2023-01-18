#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "commands.h"
Pid_table *frontp = NULL;
int proc_num_g=1;

Pid_table *create_tableNode(char *input, char * command, int pid, Pid_table *next) {
	input[strlen(input)-2]='\0';
	char pid_s[10];
	sprintf(pid_s, "%d", pid);
	char proc_num_gs[10];
	sprintf(proc_num_gs, "%d", proc_num_g);

	Pid_table *new_node = malloc(sizeof(Pid_table));
	new_node->name = (char*) malloc(sizeof(char)*strlen(input)+1);
	strcpy(new_node->name, input);
	new_node->command = (char*) malloc(sizeof(char)*strlen(command)+1);
	strcpy(new_node->command, command);
	new_node->pid_value = (char *) malloc(sizeof(char) *strlen(pid_s)+1);
	strcpy(new_node->pid_value, pid_s);
	new_node->proc_num = (char *)malloc(sizeof(char)*strlen(proc_num_gs)+1);
	strcpy(new_node->proc_num, proc_num_gs);

	new_node->next=next;

	return new_node;

}

int pid_table_insert(Pid_table **front_ptr, char *input, char * command, int pid) {
	if(*front_ptr == NULL) {
		*front_ptr=create_tableNode(input, command, pid, NULL);
		proc_num_g+=1;
		return 0;
	}
	else {
		Pid_table *curr = *front_ptr;
		while(curr->next != NULL){
			curr=curr->next;
		}

		Pid_table *new_node=create_tableNode(input, command, pid, NULL);
		curr->next = new_node;
		proc_num_g+=1;
	}
	return 0;

}

int delete_pidtableNode(int pid, Pid_table **front_ptr) {
	char pid_s[10];
	sprintf(pid_s, "%d", pid);


	Pid_table *curr = *front_ptr;

	if(curr->next==NULL){
		free(curr->name);
		free(curr->command);
		free(curr->pid_value);
		free(curr->proc_num);
		free(curr->next);
		free(curr);
		*front_ptr=NULL;
		proc_num_g=1;
		return 0;
	}
	if(strcmp(curr->pid_value, pid_s)==0){
		free(curr->name);
        free(curr->command);
        free(curr->pid_value);
        free(curr->proc_num);
		*front_ptr=curr->next;
		free(curr);
		return 0;
	}

	while(strcmp(curr->next->pid_value,pid_s)!=0) {
		curr=curr->next;
	}
		curr->next= curr->next->next;
	if (curr->next==NULL){
		proc_num_g-=1;
	}
	return 0;

}
