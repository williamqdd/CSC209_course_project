#ifndef __COMMANDS_H__
#define __COMMANDS_H__

int proc_num_g;
typedef struct pid_table {
	char *proc_num;
    char *pid_value;
    char *name;
	char *command;
    struct pid_table *next;
} Pid_table;
Pid_table *frontp;

Pid_table *create_tableNode(char *input, char * command, int pid, Pid_table *next);

int pid_table_insert(Pid_table **front_ptr, char *input, char * command, int pid);

int delete_pidtableNode(int pid, Pid_table **front_ptr);
#endif
